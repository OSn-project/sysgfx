#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <endian.h>
#include <base/macros.h>
#include <base/misc.h>
#include <math.h>

#include "../headers/sysgfx.h"
#include "../headers/tga_file.h"

#include "indexed.h"
#include "putpixel.h"

using namespace OSn;
using namespace OSn::GFX;

Bitmap *GFX::quantize(Bitmap *bmp, PixelFmt *target_fmt, Bitmap *out)
{
	if (bmp->format->mode != PixelFmt::RGBA) return NULL;
	if (target_fmt->mode != PixelFmt::INDEXED) return NULL;

	uint8 *quantized = (uint8 *) malloc(bmp->width * bmp->height);

	for (int16 y = 0; y < bmp->height; y++)
	{
		for (int16 x = 0; x < bmp->width; x++)
		{
			Color32 pixel_col = bmp->get_rgb(x, y);

			quantized[y * bmp->width + x] = closest_color(pixel_col, target_fmt);
		}
	}

	/* Now install the data into the output bitmap */
	if (! out) out = new Bitmap;

	out->width  = bmp->width;
	out->height = bmp->height;
	out->pitch  = bmp->width;

	out->set_format(target_fmt);
	out->set_data(quantized);

	return out;
}

void GFX::vflip(Bitmap *bmp)
{
	uint8 *flipped = (uint8 *) malloc (bmp->height * bmp->pitch);

	for (uint8 *src = bmp->bytes + (bmp->height * bmp->pitch) - bmp->pitch, *dest = flipped; bmp->bytes <= src; dest += bmp->pitch, src -= bmp->pitch)
	{
		memcpy(dest, src, bmp->pitch);
	}

	bmp->set_data(flipped);
}

Bitmap *GFX::read_tga(FILE *file, TGAMeta *meta)
{
	if (!file) return NULL;

	Bitmap *bitmap = new Bitmap;

	/* Read the header */
	TGAHeader header;
	fread(&header, sizeof(TGAHeader), 1, file);

	/* Copy values */
	if (header.is_indexed())
	{
		/* If the bitmap is indexed then we need to create its own format struct	*
		 * as each image may have a different palette.								*/

		PixelFmt *format;
		format            = (PixelFmt *) malloc(sizeof(PixelFmt));
		format->_malloced = true;

		format->bpp  = 8;
		format->bypp = 1;
		format->mode = PixelFmt::INDEXED;

		format->palette.size   = header.cmap_info.length;
		format->palette.colors = (Color32 *) malloc(header.cmap_info.length * sizeof(Color32));

		/* Read the colors from the color map and convert them to Color32s */
		const PixelFmt *cmap_fmt = TGAHeader::get_pixelfmt(header.cmap_info.bpp);

		fseek(file, sizeof(TGAHeader) + header.id_len, SEEK_SET);	// Seek to the color map (comes after the header

		for (uint16 i = 0; i < header.cmap_info.length; i++)
		{
			dword colorval = 0;
			fread(&colorval, cmap_fmt->bypp, 1, file);
			format->palette.colors[i] = PixelFmt::decode(colorval, (PixelFmt *) cmap_fmt);
		}

		bitmap->format = format;
	}
	else
	{
		bitmap->format = PixelFmt::ref((PixelFmt *) TGAHeader::get_pixelfmt(header.bpp));

		if (bitmap->format == NULL)
		{
			delete bitmap;
			return NULL;
		}
	}

	bitmap->width  = header.width;
	bitmap->height = header.height;
	bitmap->pitch  = header.width * bitmap->format->bypp;

	/* Read data into memory */
	bitmap->data = malloc(header.width * header.height * bitmap->format->bypp);
	bitmap->flags |= BMP_OWNDATA;

	fseek(file, sizeof(TGAHeader) + header.cmap_size() + header.id_len, SEEK_SET);

	if (header.img_type & TGA_ISRLE)
	{
		/* Deal with RLE */

		uint8 *pixel = bitmap->bytes;
		uint8  packet_len;

		putpx_fn put_pixval = putpixval_for(bitmap->format->bypp);	// This is a function pointer to a function that will set a pixel of the given size. This logic is done now, so that it doesn't slow down the loop.

		/* For each packet in the file */
		for (uint64 npx = bitmap->width * bitmap->height; npx > 0;)
		{
			fread(&packet_len, 1, 1, file);

			bool is_rle = packet_len & 0b10000000;

			packet_len &= 0b01111111;	// Get rid of the rle byte now that we know.
			packet_len += 1;			// Run lengths are stored as one less than in real.
			packet_len = b_min(npx, packet_len);	// Clip to the amount of pixels left to fill. This prevents a malicious TGA from making us write beyond memory.

			/* Expand the packet */
			if (is_rle)	// If it's a multi-pixel packet
			{
				dword pixval;
				fread(&pixval, bitmap->format->bypp, 1, file);

				for (int i = packet_len; i > 0; i--)
				{
					put_pixval(pixel, pixval);
					pixel += bitmap->format->bypp;
				}
			}
			else
			{
				fread(pixel, bitmap->format->bypp, packet_len, file);

				pixel += packet_len * bitmap->format->bypp;
			}

			npx -= packet_len;
		}

		/* TODO: Security holes:
		 *  - handle crossing of lines if pitch != w * h * bypp
		 *  - last dword goes over end of memory when expanding RLE packet
		 */
	}
	else
	{
		fread(bitmap->data, bitmap->width * bitmap->height * bitmap->format->bypp, 1, file);

		// TODO: Handle pitch if it's custom
	}

	/* Deal with image origin */
	if (!(header.image_descr & TGA_ORIGIN_TOP))
	{
		GFX::vflip(bitmap);
	}

	/* Copy TGA-specific metadata if we've been given a pointer to fill */
	if (meta != NULL)
	{
		meta->x_orig = header.x_orig;
		meta->y_orig = header.y_orig;

		/* Image ID data */
		meta->id_len = header.id_len;

		if (header.id_len > 0)
		{
			meta->img_id = malloc(header.id_len);

			fseek(file, sizeof(TGAHeader), SEEK_SET);
			fread(meta->img_id, header.id_len, 1, file);
		}
		else
		{
			meta->img_id = NULL;
		}
	}

	return bitmap;
}

Bitmap *GFX::read_tga(const char *path, TGAMeta *meta)
{
	FILE *file = fopen(path, "r");

	if (!file) return NULL;

	Bitmap *bmp = GFX::read_tga(file, meta);

	fclose(file);

	return bmp;
}

error_t GFX::write_tga(FILE *file, Bitmap *bmp, bool do_rle, TGAMeta *meta)
{
	if (!file || !bmp)
		return E_NULLPTR;

	/* Check if the pixel format is compatible with the file format */
	if (bmp->format != &tga_rgba16 &&
		bmp->format != &tga_rgb24  &&
		bmp->format != &tga_rgba32 &&
		bmp->format->mode != PixelFmt::INDEXED)
		return E_BADFMT;

	/* Compose the header */
	TGAHeader header;

	header.id_len    = (meta != NULL) ? meta->id_len : 0;
	header.cmap_type = (bmp->format->mode == PixelFmt::INDEXED) ? 1 : 0;
	header.img_type  = (bmp->format->mode == PixelFmt::INDEXED) ? TGA_INDEXED : TGA_RGBA;
	header.img_type |= (do_rle) ? TGA_ISRLE : 0;

	if (bmp->format->mode == PixelFmt::INDEXED)
	{
		header.cmap_info.first_entry_idx = 0;
		header.cmap_info.length = bmp->format->palette.size;
		header.cmap_info.bpp    = 32;	// The header will be 32-bit regardless of whether any of the palette colors have transparency or not.
	}
	else
	{
		memset(&header.cmap_info, 0x00, sizeof(header.cmap_info));
	}

	header.width  = bmp->width;
	header.height = bmp->height;

	header.x_orig = (meta != NULL) ? meta->x_orig : 0;
	header.y_orig = (meta != NULL) ? meta->y_orig : 0;

	header.bpp = bmp->format->bpp;
	header.image_descr = TGA_ORIGIN_TOP | TGA_ORIGIN_LEFT;

	/* Write the header */
	fwrite(&header, sizeof(TGAHeader), 1, file);

	/* Write the color map */
	if (bmp->format->mode == PixelFmt::INDEXED)
	{
		for (uint16 i = 0; i < bmp->format->palette.size; i++)
		{
			dword color = PixelFmt::encode(bmp->format->palette.colors[i], &tga_rgba32);
			fwrite(&color, 4, 1, file);
		}
	}

	/* Write data */
	if (do_rle)
	{
		uint8 px_sz = bmp->format->bypp;
		getpx_fn get_pixval = getpixval_for(px_sz);

		uint8 *line_start = bmp->bytes;
		uint8 *px_a, *px_b;

		for (uint16 y = 0; y < bmp->height; y++)	// For each row in the image...
		{
			px_a = line_start;		// <- This one always stays the start of the current packet's data

			while (px_a - line_start < px_sz * bmp->width)	// Until we've reached the end of the line
			{
				uint8 packet_len = 1;

				px_b = px_a + px_sz;	// next pixel along from px_a

				/* Compare the first two pixels	to	*
				 * decide what sort of packet it is */

				if (get_pixval(px_a) == get_pixval(px_b))	// Run length packet
				{
					/* Get length of run-length packet */
					while (
						get_pixval(px_a) == get_pixval(px_b)	// While the pixels stay the same...
						&& packet_len < 128 && px_b - line_start < bmp->width * px_sz	// ...and we haven't crossed the limit of the packet length or the line...
					)
					{
						packet_len++;	// increment the packet length...
						px_b += px_sz;	// ...and go one pixel further.
					}

					fwrite<uint8>((packet_len - 1) | 0b10000000, file);
					fwrite(px_a, 1, px_sz, file);
				}
				else	// Raw packet
				{
					/* Count how long data stays heterogenous */
					while (
						get_pixval(px_b - px_sz) != get_pixval(px_b)	// While the pixel before px_b != px_b ...
						&& packet_len < 128 && px_b - line_start < bmp->width * px_sz	// ...and we haven't crossed the limit of the packet length or the line...
					)
					{
						packet_len++;	// increment the packet length...
						px_b += px_sz;	// ...and go one pixel further.
					}

					fwrite<uint8>(packet_len - 1, file);
					fwrite(px_a, packet_len, px_sz, file);
				}

				px_a += px_sz * packet_len;		// Move past the pixels we've just encoded
			}

			line_start += bmp->pitch;
		}
	}
	else
	{
		/* Write raw data line-by-line */		// We can't dump the whole thing cuz it might be padded.
		for (uint8 *line = bmp->bytes; line < bmp->bytes + (bmp->pitch * bmp->height); line += bmp->pitch)
		{
			fwrite(line, bmp->width, bmp->format->bypp, file);
		}
	}

	return E_OK;
}

error_t GFX::write_tga(const char *path, Bitmap *bmp, bool rle, TGAMeta *meta)
{
	FILE *file = fopen(path, "w");

	if (!file) return E_ERROR;

	error_t rc = GFX::write_tga(file, bmp, rle, meta);

	fclose(file);

	return rc;
}

// Doesn't work properly
//void bswap_data(uint8 *src, uint8 *dest, uint32 item_count, uint8 item_size)
//{
//	/* This function splits an array into blocks `item_size` bytes in size,	*
//	 * and and flips order of the bytes within those blocks, whilst keeping	*
//	 * the order of the blocks. This function has two uses:					*
//	 *  - executing `bswapxx()` on an array of multiple values.				*
//	 *  - byte-swapping an integer of which the size is not known until		*
//	 *    runtime.															*/
//
//	for (uint8 *cur_src = src, *cur_dest = dest; cur_src - src < item_count * item_size; cur_src++, cur_dest++)
//	{
//		/* `cur_src` and `cur_dest` point to the current item that	*
//		 * we are byte-swapping.									*/
//
//		for (uint8 i = 0; i < item_size; i++)
//		{
//			cur_dest[i] = cur_src[item_size - i];
//		}
//	}
//}
#include <stdio.h>
int main(int argc, char **argv)
{
	Bitmap *bmp = GFX::read_tga(argc < 2 ? ".junk/art2.tga" : argv[1]);
	GFX::write_tga("out.tga", bmp, true);
	delete bmp;

	return 0;
}

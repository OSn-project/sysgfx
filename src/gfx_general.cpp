#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>

#include "../headers/sysgfx.h"
#include "../headers/tga_file.h"

using namespace OSn;

void bswap_data(uint8 *src, uint8 *dest, uint32 item_count, uint8 item_size);

void GFX::vflip(Bitmap *bmp)
{
	uint8 *flipped = (uint8 *) malloc (bmp->height * bmp->pitch);

	for (uint8 *src = bmp->bytes + (bmp->height * bmp->pitch) - bmp->pitch, *dest = flipped; bmp->bytes <= src; dest += bmp->pitch, src -= bmp->pitch)
	{
		memcpy(dest, src, bmp->pitch);
	}

	bmp->set(flipped);
}

// TODO: RLE support, Grayscale support, Indexed support

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
		 * as each image may have a different pallette.								*/

		PixelFmt *format;
		format       = (PixelFmt *) malloc(sizeof(PixelFmt));
		format->bpp  = header.bpp;
		format->bypp = header.bpp / 8;
		format->mode = PixelFmt::INDEXED;

		format->pallette.size   = header.cmap_info.length;
		format->pallette.colors = (Color32 *) malloc(header.cmap_info.length * sizeof(Color32));

		/* Read the colors from the color map and convert them to Color32s */
		const PixelFmt *cmap_fmt = TGAHeader::get_pixelfmt(header.cmap_info.bpp);

		fseek(file, sizeof(TGAHeader) + header.id_len, SEEK_SET);	// Seek to the color map

		for (uint16 i = 0; i < header.cmap_info.length; i++)
		{
			uint32 color = 0;
			fread(&color, cmap_fmt->bypp, 1, file);
			color = be32toh(color);
			color >>= 32 - cmap_fmt->bpp;				// A 24-bit color would be read into the lowest 3 of the 4 bytes in memory, however we want this to be the other way round.
			format->pallette.colors[i].value = PixelFmt::convert(color, cmap_fmt, &Color32::format);
		}

		/* Tell the bitmap that it owns its format pointer. */
		bitmap->format = format;
		bitmap->flags |= BMP_OWNFMT;
	}
	else
	{
		bitmap->format = (PixelFmt *) TGAHeader::get_pixelfmt(header.bpp);
	}

	bitmap->width  = header.width;
	bitmap->height = header.height;
	bitmap->pitch  = header.width * bitmap->format->bypp;

	/* Read data into memory */
	bitmap->data = malloc(header.width * header.height * bitmap->format->bypp);
	bitmap->flags |= BMP_OWNDATA;

	fseek(file, sizeof(TGAHeader) + header.cmap_size() + header.id_len, SEEK_SET);
	fread(bitmap->data, bitmap->width * bitmap->height * bitmap->format->bypp, 1, file);

	/* Deal with RLE */
	if (header.img_type & TGA_ISRLE)
	{
	}

	/* Deal with image origin */
	if (header.image_descr & TGA_ORIGIN_TOP)
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

bool GFX::write_tga(FILE *file, Bitmap *bmp, TGAMeta *meta)
{
	if (!file) return false;

	/* Compose the header */
	TGAHeader header;

	header.id_len    = (meta != NULL) ? meta->id_len : 0;
	header.cmap_type = (bmp->format->mode == PixelFmt::INDEXED) ? 1 : 0;
	header.img_type  = (bmp->format->mode == PixelFmt::INDEXED) ? TGA_INDEXED : TGA_RGBA;

	if (bmp->format->mode == PixelFmt::INDEXED)
	{
		header.cmap_info.first_entry_idx = 0;
		header.cmap_info.length = bmp->format->pallette.size;
		header.cmap_info.bpp    = 32;	// The header will be 32-bit regardless of whether any of the pallette colors have transparency or not.
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
	header.image_descr = TGA_ORIGIN_TOP; // FIXME!!!

	/* Write the header */
	fwrite(&header, sizeof(TGAHeader), 1, file);

	/* Write the color map */
	if (bmp->format->mode == PixelFmt::INDEXED)
	{
		for (uint16 i = 0; i < bmp->format->pallette.size; i++)
		{
			uint32 color = PixelFmt::convert(bmp->format->pallette.colors[i].value, &Color32::format, &tga_rgba32);
			color = htobe32(color = color);
			fwrite(&color, 4, 1, file);
		}
	}

	/* Write image data */
	uint8 *line_bswapped = (uint8 *) malloc(bmp->width * bmp->format->bypp);

	// Since the bitmap data may be padded (pitch != width * fmt->bypp), we need to write it line by line
	for (uint8 *line = bmp->bytes; line < bmp->bytes + (bmp->pitch * bmp->height); line += bmp->pitch)
	{
		if (bmp->format == &tga_rgba16)
			bswap_data(line, line_bswapped, bmp->width, bmp->format->bypp);
		else
			memcpy(line_bswapped, line, bmp->width * bmp->format->bypp);

		fwrite(line_bswapped, bmp->width, bmp->format->bypp, file);
	}

	free(line_bswapped);

	return true;
}

void bswap_data(uint8 *src, uint8 *dest, uint32 item_count, uint8 item_size)
{
	/* This function splits an array into blocks `item_size` bytes in size,	*
	 * and and flips order of the bytes within those blocks, whilst keeping	*
	 * the order of the blocks. This function has two uses:					*
	 *  - executing `bswapxx()` on an array of multiple values.				*
	 *  - byte-swapping an integer of which the size is not known until		*
	 *    runtime.															*/

	for (uint8 *cur_src = src, *cur_dest = dest; cur_src - src < item_count * item_size; cur_src++, cur_dest++)
	{
		/* `cur_src` and `cur_dest` point to the current item that	*
		 * we are byte-swapping.									*/

		for (uint8 i = 0; i < item_size; i++)
		{
			cur_dest[i] = cur_src[item_size - i];
		}
	}
}

int main(int argc, char **argv)
{
	FILE *in_file = fopen(argc > 1 ? argv[1] : ".junk/MARBLES.TGA", "r");
	Bitmap *bmp = new Bitmap(200, 100, &tga_rgba16);//GFX::read_tga(in_file);
	fclose(in_file);

	Rect rect = {20, 30, 20+10, 30+10};
	GFX::fill_rect(bmp, &rect, RGBA(0x0d0d0dff));

	FILE *out_file = fopen("out.tga", "w");
	GFX::write_tga(out_file, bmp);
	fclose(out_file);

	delete bmp;
	return 0;
}

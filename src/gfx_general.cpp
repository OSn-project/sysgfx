#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>

#include "../headers/sysgfx.h"
#include "../headers/tga_file.h"

using namespace OSn;

// Please note: Colours stored as integers are expected to be little endian.
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

status_t GFX::write_tga(FILE *file, Bitmap *bmp, TGAMeta *meta)
{
	if (!file) return E_ERROR;

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
	header.image_descr = 0; // FIXME!!!

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
	// Since the bitmap data may be padded, we need to write it line by line
	for (uint8 *data = bmp->bytes; data < bmp->bytes + (bmp->pitch * bmp->height); data += bmp->pitch)
	{
		fwrite(data, bmp->width, bmp->format->bypp, file);
	}

	return 0;
}

int main(int argc, char **argv)
{
	OSn::GFX::PixelFmt::convert(RGBA(0x44332211), &Color32::format, &tga_rgba32);

	FILE *in_file = fopen(argv[1], "r");
	Bitmap *bmp = GFX::read_tga(in_file);
	fclose(in_file);

	FILE *out_file = fopen("out.tga", "w");
	GFX::write_tga(out_file, bmp);
	fclose(out_file);

	delete bmp;
	return 0;
}

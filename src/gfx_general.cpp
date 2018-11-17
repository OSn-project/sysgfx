#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../headers/sysgfx.h"
#include "../headers/tga_file.h"

using namespace OSn;

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
		bitmap->flags |= BMP_OWNFMT;
	}
	else
	{
		bitmap->format = header.get_pixelfmt();
	}

	bitmap->width  = header.width;
	bitmap->height = header.height;
	bitmap->pitch  = header.width * bitmap->format->bypp;

	/* Read data into memory */
	bitmap->data = malloc(header.width * header.height * bitmap->format->bypp);
	bitmap->flags |= BMP_OWNDATA;

	fseek(file, sizeof(TGAHeader) + header.id_len + header.cmap_size(), SEEK_SET);
	fread(bitmap->data, bitmap->width * bitmap->height * bitmap->format->bypp, 1, file);

	/* Copy TGA-specific metadata if we've been given a pointer to fill */
	if (meta != NULL)
	{
		meta->x_orig = header.x_orig;
		meta->y_orig = header.y_orig;
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
		/* ... */
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
	header.image_descr = 32; // FIXME!!!

	/* Write the header */
	fwrite(&header, sizeof(TGAHeader), 1, file);

	/* Write image data */
	// Since the bitmap data may be padded, we need to write it line by line
	for (uint8 *data = bmp->bytes; data < bmp->bytes + (bmp->pitch * bmp->height); data += bmp->pitch)
	{
		fwrite(data, bmp->width, bmp->format->bypp, file);
	}

	return 0;
}

int main()
{
	FILE *in_file = fopen(".junk/test2_rgb.tga", "r");//fopen("/tmp/rgba.tga", "r");
	Bitmap *bmp = GFX::read_tga(in_file);
	fclose(in_file);

	FILE *out_file = fopen("out.tga", "w");
	GFX::write_tga(out_file, bmp);
	fclose(out_file);

	delete bmp;
	return 0;
}

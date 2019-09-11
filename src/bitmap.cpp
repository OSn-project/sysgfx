#include <new>
#include <stdlib.h>
#include <osndef.h>
#include <string.h>
#include <assert.h>
#include <base/misc.h>

#include "../headers/bitmap.h"

using namespace OSn;
using namespace OSn::GFX;

Bitmap :: Bitmap()
{
	this->width  = 0;
	this->height = 0;
	this->pitch  = 0;
	this->flags  = 0b00000000;

	this->format = NULL;
	this->data   = NULL;
}

Bitmap :: Bitmap(uint32 width, uint32 height, const PixelFmt *fmt)
{
	void *data = malloc(width * height * fmt->bypp);

	new (this) Bitmap(width, height, fmt, data);
}

Bitmap :: Bitmap(uint32 width, uint32 height, const PixelFmt *fmt, owning void *data)
{
	this->width  = width;
	this->height = height;
	this->pitch  = width * fmt->bypp;
	
	this->flags  = BMP_OWNDATA;
	this->data   = data;

	this->format = PixelFmt::ref((PixelFmt *) fmt);
}

Bitmap :: ~Bitmap()
{
	Bitmap::delete_fmt(this);
	Bitmap::delete_data(this);
}

void Bitmap :: set_data(owning void *data)
{
	if (data != this->data && this->flags & BMP_OWNDATA)
	{
		free(this->data);

		this->data   = data;
		this->flags |= BMP_OWNDATA;
	}
}

void Bitmap :: set_data_u(void *data)
{
	if (data != this->data && this->flags & BMP_OWNDATA)
	{
		free(this->data);

		this->data   = data;
		this->flags  = ~BMP_OWNDATA & this->flags;	// We explicitly don't own the data
	}
}

void Bitmap :: set_format(PixelFmt *fmt)
{
	if (fmt != this->format)
	{
		Bitmap::delete_fmt(this);

		this->pitch  = this->width * fmt->bypp;
		this->format = PixelFmt::ref(fmt);
	}
}

void *Bitmap :: get_pixel(int16 x, int16 y) const
{
	return this->bytes + (y * this->pitch) + (x * this->format->bypp);
}

Color32 Bitmap :: get_rgb(int16 x, int16 y) const
{
	if (x < 0 || x > this->width)  return RGBA(0,0,0);
	if (y < 0 || y > this->height) return RGBA(0,0,0);

	dword *pixel = (dword *) &this->bytes[y * this->pitch + x * this->format->bypp];

	return PixelFmt::decode(*pixel, this->format);
}

void Bitmap :: set_pixel(uint8 *pixel, dword value, PixelFmt *fmt)
{
	memcpy(pixel, (uint8 *)(&value), fmt->bypp);

//	uint32 *pixel = (uint32 *) _pixel;
//	uint8 fmt_bpp = 8 * fmt->bypp;		// This number of bits per pixel will always be a multiple of 8, even if ->bpp is something like 15.
//	*pixel = (*pixel & (0xffffffff << fmt_bpp))	// Keep any bytes of the dword that don't belong to the pixel
//	       | (value);
//	*pixel = (*pixel & (0xffffffff >> fmt_bpp))	// Keep any bytes of the dword that don't belong to the pixel
//	       | (value << (32 - fmt_bpp));

//	uint32 *pixel = (uint32 *) _pixel;
//	uint8 fmt_bpp = 8 * fmt->bypp;		// This number of bits per pixel will always be a multiple of 8, even if ->bpp is something like 15.
//
//	*pixel = (*pixel & (0xffffffff >> fmt_bpp))	// Keep any bytes of the dword that don't belong to the pixel
//	       | (value << (32 - fmt_bpp));

//	uint32 bytes = htobe32(value);
//	memcpy(pixel, &bytes + (4 - fmt->bypp), fmt->bypp);

//	/* The integer we've been passed will have the pixel value	*
//	 * stored in its lowest-order bytes. We need to copy ONLY	*
//	 * these bytes into the given memory location. Say for a 	*
//	 * format with 3 bytes per pixel we would only copy the 	*
//	 * lowest-order three of the four bytes. Because system-	*
//	 * endianness may vary, we have to resort to bit-shifting.	*/
//
//	for (uint8 byte_no = 0; byte_no < fmt->bypp; byte_no++)
//	{
//		pixel[byte_no] = (uint8) value >> (8 * byte_no);
//	}
}

//static dword lbytes(uint8 n)
//{
//	union {
//		uint8 bytes[4];
//		dword val;
//	} out;
//
//	switch (n)
//	{
//		case 0: out.bytes = {0x00, 0x00, 0x00, 0x00}; break;
//		case 1: out.bytes = {0xff, 0x00, 0x00, 0x00}; break;
//		case 2: out.bytes = {0xff, 0xff, 0x00, 0x00}; break;
//		case 3: out.bytes = {0xff, 0xff, 0xff, 0x00}; break;
//		case 4: out.bytes = {0xff, 0xff, 0xff, 0xff}; break;
//	}
//
//	return out.val;
//}
//
//static dword hbytes(uint8 n)
//{
//	union {
//		uint8 bytes[4];
//		dword val;
//	} out;
//
//	switch (n)
//	{
//		case 0: out.bytes = {0x00, 0x00, 0x00, 0x00}; break;
//		case 1: out.bytes = {0x00, 0x00, 0x00, 0xff}; break;
//		case 2: out.bytes = {0x00, 0x00, 0xff, 0xff}; break;
//		case 3: out.bytes = {0x00, 0xff, 0xff, 0xff}; break;
//		case 4: out.bytes = {0xff, 0xff, 0xff, 0xff}; break;
//	}
//
//	return out.val;
//}

Bitmap *Bitmap :: convert(const Bitmap *src, PixelFmt *fmt, Bitmap *out)
{
	if (! src) return NULL;
	if (! fmt) return NULL;

	uint8 *converted = (uint8 *) malloc(src->width * fmt->bypp * src->height);

	if (src->pitch == src->width * src->format->bypp)
	{
		/* If there is no padding, convert in one go. */
		PixelFmt::convert_pixels(src->data, converted, src->width * src->height, src->format, fmt);
	}
	else
	{
		/* If there's padding, we need to convert it line by line. */

		for (int32 y = 0; y < src->height; y++)
		{
			PixelFmt::convert_pixels(src->bytes + (y * src->pitch), converted + (src->width * fmt->bypp), src->width, src->format, fmt);
		}
	}

	/* Now install the data into the output bitmap */
	if (out)
	{
		out->set_data(converted);
		out->set_format(fmt);
	}
	else
	{
		out = new Bitmap(src->width, src->height, fmt, converted);
	}

	return out;
}

void Bitmap :: delete_fmt(Bitmap *bmp)
{
	if (bmp->format == NULL) return;

	PixelFmt::unref(bmp->format);
}

void Bitmap :: delete_data(Bitmap *bmp)
{
	if (bmp->flags & BMP_OWNDATA)
	{
		free(bmp->data);
	}
}

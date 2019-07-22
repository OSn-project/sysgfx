#include <stdlib.h>
#include <osndef.h>
#include <string.h>
#include <base/misc.h>

#include "../headers/bitmap.h"

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

Bitmap :: Bitmap(uint32_t width, uint32_t height, const PixelFmt *fmt)
{
	this->width  = width;
	this->height = height;
	this->pitch  = width * fmt->bypp;
	
	this->flags  = BMP_OWNDATA;
	this->format = (PixelFmt *) fmt;
	
	this->data = (uint8 *) malloc(width * height * fmt->bypp);
}

Bitmap :: ~Bitmap()
{
	if (this->flags & BMP_OWNFMT)	free((void *) this->format);
	if (this->flags & BMP_OWNDATA)	free(this->data);
}

void Bitmap :: set(void *data)
{
	if (this->flags & BMP_OWNDATA)
		free(this->data);

	this->data = data;
}

void *Bitmap :: get_pixel(uint32 x, uint32 y) const
{
	return this->bytes + (y * this->pitch) + (x * this->format->bypp);
}

bool Bitmap :: get_pixel(uint32 x, uint32 y, dword *out)
{
	void *pixel = this->bytes + (y * this->pitch) + (x * this->format->bypp);

	memcpy(out + (4 - this->format->bypp), pixel, this->format->bypp);
//	*out = (pixel - 4 + this->format.bypp)			// Align so that the bytes of the pixel are on the right of the dword

	return true;
}

void Bitmap :: set_pixel(uint8 *_pixel, PixelFmt *fmt, dword value)
{
	memcpy(_pixel, (uint8 *)(&value) + (4 - fmt->bypp), fmt->bypp);

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

Bitmap *Bitmap :: convert(const Bitmap *src, const PixelFmt *fmt, Bitmap *out)
{
	if (! src) return NULL;
	if (! fmt) return NULL;

	if (out == NULL)
	{
		out = new Bitmap(src->width, src->height, fmt);		// TODO: Format is not changed if out bitmap is given.
	}														// TODO: Pixel mem needlessly allocated just to be replaced later

	uint8 *converted = (uint8 *) malloc(src->width * fmt->bypp * src->height);

	dword in_val, out_val;

	for (int32 y = 0; y < src->height; y++)
	{
		for (int32 x = 0; x < src->width; x++)
		{
			memcpy((uint8 *) &in_val + (4 - src->format->bypp), src->bytes + (y * src->pitch) + (x * src->format->bypp), src->format->bypp);
			out_val = PixelFmt::convert(in_val, src->format, fmt);
			memcpy(converted + (y * src->width * fmt->bypp) + (x * fmt->bypp), (uint8 *) &out_val + (4 - fmt->bypp), fmt->bypp);
		}
	}

	out->set(converted);

	return out;
}

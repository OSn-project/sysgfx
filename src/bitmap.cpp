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

void Bitmap :: set(uint8 *pixel, PixelFmt *fmt, uint32 value)
{
	uint32 bytes = htobe32(value);
	memcpy(pixel, &bytes + (4 - fmt->bypp), fmt->bypp);

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

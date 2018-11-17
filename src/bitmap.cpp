#include "../headers/bitmap.h"

#include <stdlib.h>
#include <osndef.h>
#include <base/misc.h>


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
	
	this->format = fmt;
	
	this->data = (uint8 *) malloc(b_ceil(width * height * fmt->bpp, 8));
}

Bitmap :: ~Bitmap()
{
	if (this->flags & BMP_OWNFMT)	free((void *) this->format);
	if (this->flags & BMP_OWNDATA)	free(this->data);
}

void *Bitmap :: get_pixel(uint32 x, uint32 y) const
{
	return ((uint8 *) this->data) + (y * this->pitch) + (x * this->format->bypp);
}

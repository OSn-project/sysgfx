#include <stdlib.h>
#include <osndef.h>
#include <base/misc.h>

#include "../headers/bitmap.hh"

using namespace OSn;

Bitmap :: Bitmap(const BmpInfo *info, uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;
	
	this->format_info = info;
	
	this->data = (uint8 *) malloc(ceil_div(width * height * info->bpp, 8));
}

Bitmap :: ~Bitmap()
{
	free(this->data);
}

void Bitmap :: get_pixel(uint32 x, uint32 y, uint32 *out)
{
	/* Copies the given pixel into the highest `format_info->bpp` bits of `out` */
	
	uint32 *pixel = this->data + ceil_div((y * this->width * this->format_info->bpp) + (x * this->format_info->bpp), 8)
}

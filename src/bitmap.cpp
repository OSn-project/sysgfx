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

	this->x1 = 0; this->y1 = 0;
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

	this->x1 = 0; this->y1 = 0;
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

void Bitmap :: delete_data(Bitmap *bmp)
{
	if (bmp->flags & BMP_OWNDATA)
	{
		free(bmp->data);
	}
}

void Bitmap :: delete_fmt(Bitmap *bmp)
{
	if (bmp->format == NULL) return;

	PixelFmt::unref(bmp->format);
}

bool Bitmap :: lock()
{
	return true;	// Success
}

void Bitmap :: unlock()
{
}

bool Bitmap :: needs_lock(const Bitmap *bmp)
{
	return false;
}

//owning Bitmap *Bitmap :: copy(const Bitmap *bmp, Rect *area = NULL, uint32 flags = 0)
//{
//	Bitmap *bmp = new Bitmap()
//}

owning Bitmap *Bitmap::viewport(Rect *area)
{
	if (! area) area = &this->rect;

	Rect::clip(area, &this->rect);

	Bitmap *bmp = new Bitmap(area->w, area->h, this->format);

	bmp->bytes = this->bytes + (area->y * this->pitch) + (area->x * this->format->bypp);
	bmp->pitch = this->pitch;

	bmp->flags = this->flags & ~BMP_OWNDATA;

	return bmp;
}

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

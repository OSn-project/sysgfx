/*
 * gfx_drawing.cpp
 *
 *  Created on: Nov 30, 2018
 *      Author: albert
 */

#include <osndef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <base/macros.h>

#include "../headers/sysgfx.h"
#include "../headers/color.h"
#include "../headers/pixelfmt.h"

using namespace OSn;

void GFX::fill_rect(Bitmap *bmp, const Rect *rect, uint32 color)
{
	uint16 x1, y1, x2, y2;

	if (rect)
	{
		x1 = b_min(rect->x1, bmp->width  - 1);
		y1 = b_min(rect->y1, bmp->height - 1);
		x2 = b_min(rect->x2, bmp->width  - 1);
		y2 = b_min(rect->y2, bmp->height - 1);
	}
	else
	{
		x1 = 0;
		y1 = 0;
		x2 = bmp->width - 1;
		y2 = bmp->height - 1;
	}

	dword value = PixelFmt::convert(color, &Color32::format, bmp->format);

	for (uint32 y = y1; y <= y2; y++)
	{
		uint8 *pixel = bmp->bytes + (y * bmp->pitch) + (x1 * bmp->format->bypp);

		for (uint32 x = x1; x <= x2; x++)
		{
			Bitmap::set_pixel(pixel, bmp->format, value);
			pixel += bmp->format->bypp;
		}
	}
}

/*
 * gfx_drawing.cpp
 *
 *  Created on: Nov 30, 2018
 *      Author: albert
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../headers/sysgfx.h"
#include "../headers/color.h"
#include "../headers/pixelfmt.h"

using namespace OSn;

void GFX::fill_rect(Bitmap *bmp, const Rect *rect, uint32 color)
{
	uint32 x1 = rect ? rect->x1 : 0;
	uint32 y1 = rect ? rect->y1 : 0;
	uint32 x2 = rect ? rect->x2 : bmp->width;
	uint32 y2 = rect ? rect->y2 : bmp->height;

	uint32 value = PixelFmt::convert(color, &Color32::format, bmp->format);

	for (uint32 y = y1; y <= y2; y++)
	{
		uint8 *pixel = bmp->bytes + (y * bmp->pitch);

		for (uint32 x = x1; x < x2; x++)
		{
			Bitmap::set(pixel, bmp->format, value);
			pixel += bmp->format->bypp;
		}
	}
}


#ifndef __OSn_BITMAP32_H__
#define __OSn_BITMAP32_H__

#include <base.h>

#include "rect.h"
#include "color.h"

#ifdef __cplusplus
extern "C" {
#endif

OSn_Bitmap      *bmp32_new(uint32 width, uint32 height);

void             bmp_set_color32(OSn_Bitmap *bmp, int32 x, int32 y, int32 color32);	// The color32 argument is cast to an int32 so that you don't have to give a pointer, to make static colors easier.
int32            bmp_get_color32(OSn_Bitmap *bmp, int32 x, int32 y);

/*  */
extern const static OSn_BitmapFormat __bmp32_format;

#ifdef __cplusplus
}
#endif
#endif

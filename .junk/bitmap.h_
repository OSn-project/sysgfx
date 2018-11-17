#ifndef __OSn_BITMAP_H__
#define __OSn_BITMAP_H__

#include <stdbool.h>
#include <osndef.h>

#include "rect.h"
#include "color.h"
#include "pixelinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BMP_OWNING	0b00000001		// Whether the struct should free the image data upon destruction

typedef struct
{
	int32 width;
	int32 height;
	int32 pitch;
	
	const OSn_PixelInfo *fmt_info;	// The pixel format of the bitmap. *Not* freed upon destruction.
	
	uint8 flags;
	
	uint8 *data;
} OSn_Bitmap;

OSn_Bitmap *bmp_new(const OSn_PixelInfo *fmt, uint32 width, uint32 height);
void        bmp_init(OSn_Bitmap *bmp, const OSn_PixelInfo *fmt, uint32 width, uint32 height, uint8 *data);
void        bmp_free(OSn_Bitmap *bmp);

void        bmp_get_pixel(OSn_Bitmap *bmp, int32 x, int32 y, uint32 *out);

//OSn_Bitmap      *gfx_load(FILE *tga_file);	// -> gfx.h/imagefiles.h

#ifdef __cplusplus
}
#endif

#endif

#ifndef OSn_SYSGFX_H_
#define OSn_SYSGFX_H_

#include <osndef.h>

#include "rect.h"
#include "bitmap.h"

#include "sysgfx_errors.h"

typedef struct _IO_FILE FILE;
struct TGAMeta;

namespace OSn
{
	namespace GFX
	{
		class Bitmap;

		/* Copying data */
		void blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);
		void hw_blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);

		/* Manipulation */
		//void hflip(Bitmap *bmp);
		void vflip(Bitmap *bmp);

		/* Drawing */
		void fill_rect(Bitmap *bmp, const Rect *rect, uint32 color);	// Fill the given area on the image with the given color. Pass NULL to fill the whole image. Area includes the given `x2` and `y2`.
		void draw_rect(Bitmap *bmp, const Rect *rect, uint32 color, uint32 borderw = 1);

		void line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);
		void aa_line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);

		/* File I/O */
		Bitmap *read_tga(FILE *file, TGAMeta *meta = NULL);
		bool    write_tga(FILE *file, Bitmap *bmp, TGAMeta *meta = NULL/*, bool rle = false*/);

		/* Misc. */
		bool palette_safe(const Bitmap *bmp);	// Checks whether all pixel values in the bitmap refer to colors within the palette. Always returns `true` for non-indexed bitmaps. This function can be used to ensure that a bitmap with an 8 color palette does not have a pixel with a palette index of 10.
	}
}

// + Font rendering
// + packed bitmaps
// + TGA RLE compression

#endif

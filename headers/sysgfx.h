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
		class  Bitmap;
		struct PaintHooks;

		/* Copying data */
		void blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect) __attribute__((nonnull (1, 3)));
//		void hw_blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);

		/* Colour conversion */
		Bitmap *quantize(Bitmap *bmp, PixelFmt *palette, Bitmap *out = NULL);	// Allocated a new bitmap if `out` is NULL.

		/* Manipulation */
		//void hflip(Bitmap *bmp);
		void vflip(Bitmap *bmp);

		/* Drawing */
		void draw_line(Bitmap *bmp, int16 x1, int16 y1, int16 x2, int16 y2, Color32 color);

		void draw_rect(Bitmap *bmp, const Rect *rect, Color32 color, int16 borderw = 1);
		void fill_rect(Bitmap *bmp, const Rect *rect, Color32 color);	// Fill the given area on the image with the given color. Pass NULL to fill the whole image. Area includes the given `x2` and `y2`.

		void set_painter(PaintHooks *hooks);	// NULL resets to default. Sets for all threads.

		namespace Painters
		{
			extern PaintHooks software;		// This is the default, so all fields will have an implementation.
			extern PaintHooks software_aa;
		}

		/* File I/O */
		Bitmap  *read_tga(FILE *file,       TGAMeta *meta = NULL);
		Bitmap  *read_tga(const char *path, TGAMeta *meta = NULL);
		error_t  write_tga(FILE *file,       Bitmap *bmp, bool rle = false, TGAMeta *meta = NULL);
		error_t  write_tga(const char *path, Bitmap *bmp, bool rle = false, TGAMeta *meta = NULL);

		/* Misc. */
		bool palette_safe(const Bitmap *bmp);	// Checks whether all pixel values in the bitmap refer to colors within the palette. Always returns `true` for non-indexed bitmaps. This function can be used to ensure that a bitmap with an 8 color palette does not have a pixel with a palette index of 10.

	}
}

// + Font rendering
// + packed bitmaps
// + TGA RLE compression

#endif

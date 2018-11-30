#ifndef OSn_SYSGFX_H_
#define OSn_SYSGFX_H_

#include <osndef.h>

#include "rect.h"
#include "bitmap.h"

#include "sysgfx_errors.h"

typedef uint32 status_t;
#define E_ERROR 1
#define E_BAD_IMGFMT 6624

typedef struct _IO_FILE FILE;
struct TGAMeta;

namespace OSn
{
	namespace GFX
	{
		class Bitmap;

		void blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);
		void hw_blit(const Bitmap *src, const Rect *src_rect, Bitmap *dest, const Rect *dest_rect);

		/* Drawing */
		void fill_rect(Bitmap *bmp, const Rect *rect, uint32 color);
		void draw_rect(Bitmap *bmp, const Rect *rect, uint32 color, uint32 width = 1);

		void line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);
		void aa_line(Bitmap *bmp, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 color, uint32 width = 1);

		Bitmap  *read_tga(FILE *file, TGAMeta *meta = NULL);
		status_t write_tga(FILE *file, Bitmap *bmp, TGAMeta *meta = NULL/*, bool rle = false*/);
	}
}



#endif

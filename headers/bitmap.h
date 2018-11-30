#ifndef OSn_BITMAP_H_
#define OSn_BITMAP_H_

#include <osndef.h>

#include "rect.h"
#include "color.h"
#include "pixelfmt.h"

namespace OSn
{
	namespace GFX
	{
		class Raster
		{
		public:
			uint32 width, height;
		
		public:
			virtual void *get_pixel(uint32 x, uint32 y) const = 0;

			virtual ~Raster() {};
		};
		
		/*  */
		#define BMP_OWNDATA 0b00000001
		#define BMP_OWNFMT  0b00000010
		
		class Bitmap : public Raster
		{
		public:
			uint8  flags;
			uint32 pitch;

			PixelFmt *format;	// Unless the BMP_OWNFMT flag is set, the referenced struct needs to be treated as a `const`.

			union
			{
				uint8   *bytes;
				uint32  *dwords;
				Color32 *colors;
				void    *data;
			};

		public:
			Bitmap();
			Bitmap(const Bitmap *bmp);
			Bitmap(const Bitmap *bmp, const Rect *area);
			Bitmap(uint32 width, uint32 height, const PixelFmt *fmt);
			~Bitmap();

			void set(void *data);

			void *get_pixel(uint32 x, uint32 y) const;
			inline bool is_indexed() const { return this->format->mode == PixelFmt::INDEXED; }
		};
		
		struct Fragment
		{
			/* Used to refer to the area of a bitmap */

			const Bitmap *source;
			Rect rect;
		};
	}
	
	using GFX::Bitmap;
	using GFX::Fragment;
}

#endif

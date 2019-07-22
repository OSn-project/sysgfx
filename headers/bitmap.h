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
//				Color32 *colors;
				void    *data;
			};

		public:
			Bitmap();
//			Bitmap(const Bitmap *bmp);
//			Bitmap(const Bitmap *bmp, const Rect *area);
			Bitmap(uint32 width, uint32 height, const PixelFmt *fmt);
			~Bitmap();

			inline bool integrity() const { return (this->format != NULL && this->data != NULL); }
			inline bool is_indexed() const { return this->format->mode == PixelFmt::INDEXED; }

			void set(void *data);

			void *get_pixel(uint32 x, uint32 y) const;		// Returns a pointer to the memory location of the pixel at the given coordinates. Returns NULL for out-of-bounds coordinates.
			bool  get_pixel(uint32 x, uint32 y, dword *out);	// Copy the value of the given pixel into the highest-in-memory bytes of the given integer. Does not convert format. Returns false for out-of-bounds coordinates.
			static void set_pixel(uint8 *pixel, PixelFmt *fmt, dword value);	// Sets the pixel at the given location in memory to the given

			void to_rgb(PixelFmt *fmt, Bitmap *out) const;		// This is always a lossless operation, unlike `rgb -> indexed`.
			static Bitmap *convert(const Bitmap *source, const PixelFmt *fmt, Bitmap *out = NULL);	// Converts the source bitmap's data to the given format. Returns a pointer to the resulting image (must be freed). Destination image may be set using `out` argument. Returns NULL if either source or destination format is indexed.

			static void delete_fmt(Bitmap *bmp);
			static void delete_data(Bitmap *bmp);
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

// TODO: Lockable bitmap
// TODO: Ask bitmap to allocate its own memory for given witdth height size (so that it can set its own pitch, etc.)

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
		/*  */
		#define BMP_OWNDATA 0b00000001
		
		class Bitmap
		{
		public:
			union {								// This saves you from typing `rect.` when accessing them independently.
				Rect rect;
				struct { int16 x1, y1; int16 width, height; };
			};

			uint32 pitch;
			uint8  flags;

			PixelFmt *format;	// This is reference-counted but the Bitmap class takes care of this for us.

			union
			{
				uint8   *bytes;
				dword   *dwords;
				void    *data;
			};

		public:
			Bitmap();	// Initializes a bitmap with no buffer or pixel format
			Bitmap(uint32 width, uint32 height, const PixelFmt *fmt);		// Initializes a bitmap with a buffer of the specified dimensions of the given pixel format. Buffer is not cleared to 0s by default. Increments the reference count of `fmt`.
			Bitmap(uint32 width, uint32 height, const PixelFmt *fmt, owning void *data);
			virtual ~Bitmap();

			inline bool integrity() const { return (this->format != NULL && this->data != NULL); }
			inline bool is_indexed() const { return this->format->mode == PixelFmt::INDEXED; }

			void set_data(void *data);
			void set_data_u(void *data);		// Un/not-owning version of `set_data`
			void set_format(PixelFmt *fmt);		// Change the pixel format of the bitmap. Takes care of changing reference counts.

			static void delete_data(Bitmap *bmp);	// Frees the data if we own it.
			static void delete_fmt(Bitmap *bmp);	// Unrefs the format so that it gets freed if we're the only ones referencing it.

			bool lock();	// Locking functions. These are stubs for now.
			void unlock();
			static bool needs_lock(const Bitmap *bmp);

			static owning Bitmap *copy(const Bitmap *bmp, Rect *area = NULL, uint32 flags = 0);
			owning Bitmap *viewport(Rect *area);

//			MOVE TO GFX:: Bitmap *to_rgb(PixelFmt *fmt, Bitmap *out = NULL) const;		// This is always a lossless operation, unlike `rgb -> indexed`.
			static Bitmap *convert(const Bitmap *source, PixelFmt *fmt, Bitmap *out = NULL);	// Converts the source bitmap's data to the given format. Returns a pointer to the resulting image (must be freed). Destination image may be set using `out` argument. Returns NULL if either source or destination format is indexed.
		};
	}
	
	using GFX::Bitmap;
}

#endif

// TODO: Lockable bitmap
// TODO: Ask bitmap to allocate its own memory for given witdth height size (so that it can set its own pitch, etc.)

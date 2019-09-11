#ifndef OSn_GFX_PIXELFMT_H_
#define OSn_GFX_PIXELFMT_H_

#include <osndef.h>
#include "color.h"

/*
 * We have to be careful when manipulating pixel values as we don't know whether the machine is big- or little-endian.
 * This limits us in a couple of ways:
 *  - We can't assign literal values to ints, as the order of the number's bytes will vary according to the system. `a = 0x12345678` may be stored as `12 34 56 78` on one system, but will be `78 56 34 12` on another.
 *  - Pixel values are stored as `dword`s. Although this is an `int32` under the hood, it should be treated like a 4-byte array.
 *  - The use of some operations, such as bit-shifting* is limited. Bit-wise operations such as AND and XOR are safe.
 *
 * *bit-shifting is only safe on single bytes (uint8)
 */

namespace OSn
{
	namespace GFX
	{
		struct PixelFmt
		{
			/* Used to represent the pixel format	*
			 * of a bitmap.							*/

			/* THIS STRUCT USES REFERENCE COUNTING.	*
			 * If you `malloc` it, it automatically	*
			 * has one reference to it; just instead*
			 * of doing `free` do `PixelFmt::unref`.*
			 * If you initialized its values in a 	*
			 * file, you don't need to do any		*
			 * deallocation, just like normal.		*/

			uint8 bpp;			// The number of bits per pixel.
			uint8 bypp;			// The number of bytes per pixel. Should be bpp // 8 rounded up. Only 1 - 4 supported.
			
			enum { RGBA, INDEXED, CUSTOM } mode;

			union
			{
				struct
				{
					/* Red channel */
					uint8 r_mask[4];	// A mask representing the red component of the pixel value. The bytes of this integer are stored in the order as displayed.
					uint8 r_shift;		// The number of bits to the right of the component in each pixel value
					uint8 r_size;		// The size of the channel in bits. Less or equal to 8.

					/* Green channel */
					uint8 g_mask[4];
					uint8 g_shift;
					uint8 g_size;

					/* Blue channel */
					uint8 b_mask[4];
					uint8 b_shift;
					uint8 b_size;

					/* Alpha channel */
					uint8 a_mask[4];
					uint8 a_shift;
					uint8 a_size;

					bool byte_swap;		// Reverses the order of the bytes once the pixel value is encoded. Set for little-endian formats.
					bool invert_alpha;	// If true, 255 = transparent and 0 = opaque.
				} rgba;

				struct
				{
					uint16   size;
					Color32 *colors;
				} palette;
			};							// This union is anonymous, meaning that its members cede to the enclosing struct.

			struct {
				/* These are compulsory if the mode is `CUSTOM`.	*
				 * They may be used in other modes as well if the 	*
				 * source/target happens to be stored as `Color32`.	*/

				void (*encode_color32)(Color32 *src, void *dst, uint64 length);
				void (*decode_color32)(void *src, Color32 *dst, uint64 length);
			};

			/* Reference counting for when the format is malloced.	*
			 * Don't set these in designated initializers.			*/
			uint32 _refcount;		// Refcounting here is a bit unusial because the struct gets freed not when the count goes from 1 -> 0, but when it goes from 0 -> -1. It could rather be thought of as 'number of references other than the creator reference'. This decision was made to save boilerplate code, because the struct is often initialized literally in a file, or malloced.
			bool   _malloced;		// Struct is only freed on no references when this is set to true. Implicitly set to `false` when initialized in a file.

			static PixelFmt *ref  (PixelFmt *fmt);
			static PixelFmt *unref(PixelFmt *fmt);

			static uint8 *expand_table[9];	// For expanding < 8 bit values

//			static owning PixelFmt *copy(const PixelFmt *fmt);		// Wait, there's no reason to copy a PixelFmt!	// Probably not a good idea because some functions compare pointers to check for a format. (eg. `if (bmp->format == &Color32::format) ...`)
			static bool compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b);

			static dword   encode(Color32 col, PixelFmt *out_fmt);	// If format is indexed, finds the nearest colour.
			static Color32 decode(dword val, PixelFmt *in_fmt);

			static bool    encode_pixels(Color32 *in, void *out, uint64 length, PixelFmt *fmt);
			static bool    decode_pixels(void *in, Color32 *out, uint64 length, PixelFmt *fmt);
			static bool    convert_pixels(void *in, void *out, uint64 length, PixelFmt *in_fmt, PixelFmt *out_fmt);

			static owning PixelFmt *indexed_for(Color32 *pal, size_t size);		// Create an indexed pixelfmt struct for the given colour palette.
		};
	}
}

#endif

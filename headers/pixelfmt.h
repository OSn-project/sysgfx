#ifndef OSn_GFX_PIXELFMT_H_
#define OSn_GFX_PIXELFMT_H_

#include <osndef.h>
#include "color.h"

/*
 * We have to be careful when manipulating pixel values as we don't know whether the machine is big- or little-endian.
 * This limits us in a coupke of ways:
 *  - We can't assign literal values to ..s, as the order of the number's bytes will vary according to the system. `a = 0x12345678` may be stored as `12 34 56 78` on one system, but will be `78 56 34 12` on another.
 *  - Pixel values are stored as `dword`s. Although this is an `int32` under the hood, it should be thought of as a 4-byte array.
 *  - Code should stay portable accross big- and little-endian systems. This means that the use of some operations, such as bit-shifting* is limited. Bit-wise operations such as AND and XOR are safe.
 *
 * *bit-shifting is only safe on single bytes (uint8)
 */

namespace OSn
{
	namespace GFX
	{
		struct Palette
		{
			uint16   size;
			Color32 *colors;
		};

		struct RGBADef		// Information needed to extract the color channels from a binary pixel value.
		{
			/* Red channel */
			uint8 r_mask[4];	// A mask representing the red component of the pixel value. The bytes of this integer are stored in the order as displayed. (order of descending significance)
			uint8 r_shift;		// The number of bits to the right of the component in each pixel value
			uint8 r_size;		// The size of the pixel value in bits. Less or equal to 8.

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

			bool byte_swap;		// Reverses the order of the bytes once the pixel value is encoded. Set for little-endian formats. Works for .bypp = 1, 2, 3, 4
			bool invert_alpha;	// If true, 255 = transparent and 0 = opaque.
		};

		struct PixelFmt
		{
			/* Used to represent the pixel format	*
			 * of a bitmap.							*/

			uint8 bpp;			// The number of bits per pixel.
			uint8 bypp;			// The number of bytes per pixel. Should be bpp // 8 rounded up.
			
			enum { INDEXED, RGBA } mode;

			union
			{
				RGBADef rgba;
				Palette palette;
			};							// This union is anonymous, meaning that its members cede to the enclosing struct.

			static uint8 *expand_table[9];

			static owning PixelFmt *copy(const PixelFmt *fmt);
			static bool compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b);

			static Color32 decode(dword val, const PixelFmt *in_fmt);		// Both encode and decode expect the format to not be indexed.
			static dword   encode(Color32 col, const PixelFmt *out_fmt);
			static dword   convert(dword val, const PixelFmt *fmt_a, const PixelFmt *fmt_b);	// If the pixel format is less than 32 bpp, the value will occupy the uppermost in memory bits/bytes.
//			static void convert(uint8 *val_a, const PixelFmt *fmt_a, uint8 *val_b, const PixelFmt *fmt_b);

			static owning PixelFmt *indexed_for(Palette *pal);		// Create an indexed pixelfmt struct for the given colour palette.
		};
	}
}

#endif

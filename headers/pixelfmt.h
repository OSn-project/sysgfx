#ifndef OSn_GFX_PIXELFMT_H_
#define OSn_GFX_PIXELFMT_H_

#include <osndef.h>
#include "color.h"

namespace OSn
{
	namespace GFX
	{
		struct PixelFmt
		{
			/* Used to represent the pixel format	*
			 * of a bitmap.							*/

			uint8 bpp;			// The number of bits per pixel.
			uint8 bypp;			// The number of bytes per pixel. Should equal `b_ceil(bpp, 8)`.
			
			enum { INDEXED, RGBA } mode;
			
			union
			{
				struct
				{
					/* Red channel */
					uint32 r_mask;		// A mask representing the red component of the pixel value. The bytes of this integer are stored in the order as displayed. (order of descending significance)
					uint8  r_shift;		// The number of bits to the right of the component in each pixel value
					uint8  r_size;		// The size of the pixel value in bits
		
					/* Green channel */
					uint32 g_mask;
					uint8  g_shift;
					uint8  g_size;
		
					/* Blue channel */
					uint32 b_mask;
					uint8  b_shift;
					uint8  b_size;
		
					/* Alpha channel */
					uint32 a_mask;
					uint8  a_shift;
					uint8  a_size;
				} rgba;
				
				struct
				{
					uint16   size;
					Color32 *colors;
				} pallette;
			};							// This union is anonymous, meaning that its members cede to its enclosing struct.

			static uint8 *expand_table[9];

			static bool compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b);

			static Color32 decode(dword val, const PixelFmt *in_fmt);
			static dword   encode(Color32 col, const PixelFmt *out_fmt);
			static dword   convert(dword val, const PixelFmt *fmt_a, const PixelFmt *fmt_b);	// If the pixel format is less than 32 bpp, the value will occupy the lowest order bits/bytes.
//			static void convert(uint8 *val_a, const PixelFmt *fmt_a, uint8 *val_b, const PixelFmt *fmt_b);
		};
	}
}

#endif

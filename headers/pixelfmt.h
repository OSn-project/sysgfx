#ifndef OSn_GFX_PIXELFMT_H_
#define OSn_GFX_PIXELFMT_H_

#include <osndef.h>
#include "gfxdefs.h"
#include "color.h"

namespace OSn
{
	namespace GFX
	{
		struct PixelFmt
		{
			/* Used to represent the pixel format	*
			 * of a bitmap.							*/
			/* Describes the format of a color 		*
			 * stored in a `pixval`.				*/

			uint8 bpp;			// The number of bits per pixel.
			uint8 bypp;			// The number of bytes per pixel. Should be `->bpp` ceiling devided by 8. 
			
			enum { INDEXED, RGBA } mode;
			
			union
			{
				struct
				{
					/* Red chanel */
					uint32 r_mask;		// A mask representing the red component of the pixel
					uint8  r_shift;		// The number of bits to the right of the component in each pixel value
					uint8  r_loss;		// The number of bits lost from each component when packing the 8-bit color component in a pixel
		
					/* Green chanel */
					uint32 g_mask;
					uint8  g_shift;
					uint8  g_loss;
		
					/* Blue chanel */
					uint32 b_mask;
					uint8  b_shift;
					uint8  b_loss;
		
					/* Alpha chanel */
					uint32 a_mask;
					uint8  a_shift;
					uint8  a_loss;
				} rgba;
				
				struct
				{
					uint16   size;
					Color32 *colors;
				} pallette;
			};							// This union is anonymous, meaning that its members cede to its enclosing struct.

			static bool compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b);
			static pixval convert(pixval pix_a, const PixelFmt *fmt_a, const PixelFmt *fmt_b);	// If the pixel format is less than 32 bpp, the value will occupy the lowest order bits/bytes.
//			static void convert(uint8 *val_a, const PixelFmt *fmt_a, uint8 *val_b, const PixelFmt *fmt_b);
		};
	}
}

#endif

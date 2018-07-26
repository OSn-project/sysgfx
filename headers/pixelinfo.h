#ifndef __OSn_RGBAINFO_H__
#define __OSn_RGBAINFO_H__

#include <osndef.h>

typedef struct
{
	/* Used to represent the pixel format	*
	 * of an RGB bitmap.					*/

	uint8 bpp;			// The number of bits per bixel

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
} OSn_PixelInfo;

#endif

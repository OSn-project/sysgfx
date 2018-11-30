#include <string.h>
#include "../headers/pixelfmt.h"

using namespace OSn;
using namespace GFX;

bool PixelFmt :: compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b)
{
	return memcmp(fmt_a, fmt_b, sizeof(PixelFmt)) == 0;
}

uint32 PixelFmt :: convert(uint32 in, const PixelFmt *in_fmt, const PixelFmt *out_fmt)
{
	uint32 out = 0;	// Both `in` and `out` variables are of system endianness

	uint8 r_byte = (in & in_fmt->rgba.r_mask) >> (in_fmt->rgba.r_shift - in_fmt->rgba.r_loss);
	uint8 g_byte = (in & in_fmt->rgba.g_mask) >> (in_fmt->rgba.g_shift - in_fmt->rgba.g_loss);
	uint8 b_byte = (in & in_fmt->rgba.b_mask) >> (in_fmt->rgba.b_shift - in_fmt->rgba.b_loss);
	uint8 a_byte = (in & in_fmt->rgba.a_mask) >> (in_fmt->rgba.a_shift - in_fmt->rgba.a_loss);

	if (in_fmt->rgba.a_mask == 0)
		a_byte = 0xff;				// If the source format does not have an alpha channel assume that the image is opaque.

	out |= (r_byte >> out_fmt->rgba.r_loss) << out_fmt->rgba.r_shift;
	out |= (g_byte >> out_fmt->rgba.g_loss) << out_fmt->rgba.g_shift;
	out |= (b_byte >> out_fmt->rgba.b_loss) << out_fmt->rgba.b_shift;
	out |= (a_byte >> out_fmt->rgba.a_loss) << out_fmt->rgba.a_shift;

	return out;
}

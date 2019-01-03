#include <string.h>
#include "../headers/pixelfmt.h"

using namespace OSn;
using namespace GFX;

bool PixelFmt :: compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b)
{
	return memcmp(fmt_a, fmt_b, sizeof(PixelFmt)) == 0;
}

uint32 mhtobe32(uint32 val) {return htobe32(val);}
uint32 mbe32toh(uint32 val) {return be32toh(val);}

uint32 PixelFmt :: convert(uint32 in/*be*/, const PixelFmt *in_fmt, const PixelFmt *out_fmt)
{
	uint32 out = 0;

	uint8 r_byte = mbe32toh(in & mhtobe32(in_fmt->rgba.r_mask)) >> (in_fmt->rgba.r_shift - in_fmt->rgba.r_loss);	// We need to convert to system endian so that bit shifts work properly. Also we promissed that the bytes of the channel mask would be interpreted in the order in which theyb are displayed, so we need to convert it to big endian.
	uint8 g_byte = mbe32toh(in & mhtobe32(in_fmt->rgba.g_mask)) >> (in_fmt->rgba.g_shift - in_fmt->rgba.g_loss);
	uint8 b_byte = mbe32toh(in & mhtobe32(in_fmt->rgba.b_mask)) >> (in_fmt->rgba.b_shift - in_fmt->rgba.b_loss);
	uint8 a_byte = mbe32toh(in & mhtobe32(in_fmt->rgba.a_mask)) >> (in_fmt->rgba.a_shift - in_fmt->rgba.a_loss);

	if (in_fmt->rgba.a_mask == 0)
		a_byte = 0xff;				// If the source format does not have an alpha channel assume that the image is opaque.

	out |= mhtobe32((r_byte >> out_fmt->rgba.r_loss) << out_fmt->rgba.r_shift);
	out |= mhtobe32((g_byte >> out_fmt->rgba.g_loss) << out_fmt->rgba.g_shift);
	out |= mhtobe32((b_byte >> out_fmt->rgba.b_loss) << out_fmt->rgba.b_shift);

	if (out_fmt->rgba.a_mask != 0)
		out |= htobe32((a_byte >> out_fmt->rgba.a_loss) << out_fmt->rgba.a_shift);

//	if (in_fmt->endian != out_fmt->endian)
//	{
//		bswap32(out);
//		out >>= 32 - (8 * out_fmt->bypp);
//	}

	return out;
}

/*void PixelFmt :: convert(uint8 *in_ptr, const PixelFmt *in_fmt, uint8 *out_ptr, const PixelFmt *out_fmt)
{
	uint32 in_val;
	memcpy(&in_val + (sizeof(uint32) - in_fmt->bypp), in_ptr, in_fmt->bypp);

	uint32 out = PixelFmt::convert(in_val, in_fmt, out_fmt);
	memcpy(out_ptr, &out + (sizeof(uint32) - in_fmt->bypp), out_fmt->bypp);
}*/

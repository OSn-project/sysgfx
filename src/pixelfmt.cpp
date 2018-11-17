#include <string.h>
#include "../headers/pixelfmt.h"

using namespace OSn;

/* FIXME: C++ designated initializers only supported by clang */

const PixelFmt GFX::Formats::rgba32 = {
	.bpp  = 32,
	.bypp = 4,
	.mode = PixelFmt::RGBA,
	
	.rgba = {
		.r_mask  = 0xff000000,
		.r_shift =         24,
		.r_loss  =          0,

		.g_mask  = 0x00ff0000,
		.g_shift =         16,
		.g_loss  =          0,

		.b_mask  = 0x0000ff00,
		.b_shift =         24,
		.b_loss  =          0,

		.a_mask  = 0x000000ff,
		.a_shift =          0,
		.a_loss  =          0,
	},
};

bool PixelFmt :: compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b)
{
	return memcmp(fmt_a, fmt_b, sizeof(PixelFmt)) == 0;
}

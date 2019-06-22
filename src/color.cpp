#include "../headers/color.h"
#include "../headers/pixelfmt.h"
#include <endian.h>
using namespace OSn;

/* FIXME: C++ designated initializers only supported by clang */

const GFX::PixelFmt Color32::format = {
	.bpp  = 32,
	.bypp = 4,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0xff000000,
		.r_shift =         24,
		.r_size  =          8,

		.g_mask  = 0x00ff0000,
		.g_shift =         16,
		.g_size  =          8,

		.b_mask  = 0x0000ff00,
		.b_shift =          8,
		.b_size  =          8,

		.a_mask  = 0x000000ff,
		.a_shift =          0,
		.a_size  =          8,
	},
};

const GFX::PixelFmt Color24::format = {
	.bpp  = 24,
	.bypp = 3,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0xff0000,
		.r_shift =       16,
		.r_size  =        8,

		.g_mask  = 0x00ff00,
		.g_shift =        8,
		.g_size  =        8,

		.b_mask  = 0x0000ff,
		.b_shift =        0,
		.b_size  =        8,
	},
};

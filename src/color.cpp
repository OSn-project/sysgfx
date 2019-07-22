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
		.r_mask  = {0xff, 0x00, 0x00, 0x00},
		.r_shift = 0,
		.r_size  = 8,

		.g_mask  = {0x00, 0xff, 0x00, 0x00},
		.g_shift = 8,
		.g_size  = 8,

		.b_mask  = {0x00, 0x00, 0xff, 0x00},
		.b_shift = 16,
		.b_size  = 8,

		.a_mask  = {0x00, 0x00, 0x00, 0xff},
		.a_shift = 24,
		.a_size  = 8,
	},
};

const GFX::PixelFmt Color24::format = {
	.bpp  = 24,
	.bypp = 3,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = {0x00, 0xff, 0x00, 0x00},
		.r_shift = 8,
		.r_size  = 8,

		.g_mask  = {0x00, 0x00, 0xff, 0x00},
		.g_shift = 16,
		.g_size  = 8,

		.b_mask  = {0x00, 0x00, 0x00, 0xff},
		.b_shift = 24,
		.b_size  = 8,
	},
};

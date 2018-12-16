#include "../headers/color.h"
#include "../headers/pixelfmt.h"

using namespace OSn;

/* FIXME: C++ designated initializers only supported by clang */

const GFX::PixelFmt Color32::format = {
	.bpp  = 32,
	.bypp = 4,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0xff000000,
		.r_shift =         24,
		.r_loss  =          0,

		.g_mask  = 0x00ff0000,
		.g_shift =         16,
		.g_loss  =          0,

		.b_mask  = 0x0000ff00,
		.b_shift =          8,
		.b_loss  =          0,

		.a_mask  = 0x000000ff,
		.a_shift =          0,
		.a_loss  =          0,
	},
};

/*const GFX::PixelFmt Color24::format = {
	.bpp  = 24,
	.bypp = 3,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0x00ff0000,
		.r_shift =         16,
		.r_loss  =          0,

		.g_mask  = 0x0000ff00,
		.g_shift =         24,
		.g_loss  =          0,

		.b_mask  = 0x000000ff,
		.b_shift =          0,
		.b_loss  =          0,
	},
};*/

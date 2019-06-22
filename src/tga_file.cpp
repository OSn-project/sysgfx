#include <stddef.h>
#include "../headers/tga_file.h"

using namespace OSn::GFX;

const PixelFmt tga_rgba16 = {
	.bpp   = 16,
	.bypp  = 2,
	.mode  = PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0b0000000000011111,
		.r_shift = 0,
		.r_size  = 5,

		.g_mask  = 0b0000001111100000,
		.g_shift = 5,
		.g_size  = 5,

		.b_mask  = 0b0111110000000000,
		.b_shift = 10,
		.b_size  = 5,

		.a_mask  = 0b1000000000000000,
		.a_shift = 15,
		.a_size  = 1,
	},
};

const PixelFmt tga_rgb24 = {
	.bpp  = 24,
	.bypp = 3,
	.mode = PixelFmt::RGBA,

	.rgba = {
		.r_mask  = 0x000000ff,
		.r_shift =          0,
		.r_size  =          8,

		.g_mask  = 0x0000ff00,
		.g_shift =          8,
		.g_size  =          8,

		.b_mask  = 0x00ff0000,
		.b_shift =         16,
		.b_size  =          8,
	},
};

const PixelFmt tga_rgba32 = {
	.bpp  = 32,
	.bypp = 4,
	.mode = PixelFmt::RGBA,

	.rgba = {
		.b_mask  = 0xff000000,
		.b_shift =         24,
		.b_size  =          8,

		.g_mask  = 0x00ff0000,
		.g_shift =         16,
		.g_size  =          8,

		.r_mask  = 0x0000ff00,
		.r_shift =          8,
		.r_size  =          8,

		.a_mask  = 0x000000ff,
		.a_shift =          0,
		.a_size  =          8,
	},
};

const PixelFmt *TGAHeader :: get_pixelfmt(uint8 bpp)
{
	if (bpp == 16) return &tga_rgba16;
	if (bpp == 24) return &tga_rgb24;
	if (bpp == 32) return &tga_rgba32;

	return NULL;
}

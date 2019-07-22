#include <stddef.h>
#include "../headers/tga_file.h"

using namespace OSn::GFX;

const PixelFmt tga_rgba16 = {
	.bpp   = 16,
	.bypp  = 2,
	.mode  = PixelFmt::RGBA,

	.rgba = {
		.r_mask  = {0, 0, 0b00000000, 0b00011111},
		.r_shift = 27,
		.r_size  = 5,

		.g_mask  = {0, 0, 0b00000011, 0b11100000},
		.g_shift = 22,
		.g_size  = 5,

		.b_mask  = {0, 0, 0b01111100, 0b00000000},
		.b_shift = 17,
		.b_size  = 5,

		.a_mask  = {0, 0, 0b10000000, 0b00000000},
		.a_shift = 16,
		.a_size  = 1,
	},
};

const PixelFmt tga_rgb24 = {
	.bpp  = 24,
	.bypp = 3,
	.mode = PixelFmt::RGBA,

	.rgba = {
		.b_mask  = {0x00, 0xff, 0x00, 0x00},
		.b_shift = 8,
		.b_size  = 8,

		.g_mask  = {0x00, 0x00, 0xff, 0x00},
		.g_shift = 16,
		.g_size  = 8,

		.r_mask  = {0x00, 0x00, 0x00, 0xff},
		.r_shift = 24,
		.r_size  = 8,
	},
};

const PixelFmt tga_rgba32 = {
		.bpp  = 32,
		.bypp = 4,
		.mode = PixelFmt::RGBA,

		.rgba = {
			.b_mask  = {0xff, 0x00, 0x00, 0x00},
			.b_shift = 0,
			.b_size  = 8,

			.g_mask  = {0x00, 0xff, 0x00, 0x00},
			.g_shift = 8,
			.g_size  = 8,

			.r_mask  = {0x00, 0x00, 0xff, 0x00},
			.r_shift = 16,
			.r_size  = 8,

			.a_mask  = {0x00, 0x00, 0x00, 0xff},
			.a_shift = 24,
			.a_size  = 8,
		},
	};

const PixelFmt *TGAHeader :: get_pixelfmt(uint8 bpp)
{
	if (bpp == 16) return &tga_rgba16;
	if (bpp == 24) return &tga_rgb24;
	if (bpp == 32) return &tga_rgba32;

	return NULL;
}

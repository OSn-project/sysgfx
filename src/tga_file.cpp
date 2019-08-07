#include <stddef.h>
#include "../headers/tga_file.h"

using namespace OSn::GFX;

/*const*/ PixelFmt tga_rgba16 = {
	.bpp   = 16,
	.bypp  = 2,
	.mode  = PixelFmt::RGBA,

	.rgba = {
		.r_mask  = {0b01111100, 0b00000000},	// Remaining 2 bytes are 0
		.r_shift = 1,
		.r_size  = 5,

		.g_mask  = {0b00000011, 0b11100000},
		.g_shift = 6,
		.g_size  = 5,

		.b_mask  = {0b00000000, 0b00011111},
		.b_shift = 11,
		.b_size  = 5,

		.a_mask  = {0b10000000, 0b00000000},
		.a_shift = 0,
		.a_size  = 1,

		.byte_swap = true,
	},
};

/*const*/ PixelFmt tga_rgb24 = {
	.bpp  = 24,
	.bypp = 3,
	.mode = PixelFmt::RGBA,

	.rgba = {
		.b_mask  = {0xff, 0x00, 0x00},
		.b_shift = 0,
		.b_size  = 8,

		.g_mask  = {0x00, 0xff, 0x00},
		.g_shift = 8,
		.g_size  = 8,

		.r_mask  = {0x00, 0x00, 0xff},
		.r_shift = 16,
		.r_size  = 8,
	},
};

/*const*/ PixelFmt tga_rgba32 = {
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
	switch (bpp)
	{
		case 16: return &tga_rgba16;
		case 24: return &tga_rgb24;
		case 32: return &tga_rgba32;

		default: return NULL;
	}
}

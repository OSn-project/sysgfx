#include "../headers/color.h"
#include "../headers/pixelfmt.h"
#include <endian.h>
using namespace OSn;

/* FIXME: C++ designated initializers only supported by clang */

/* 32-bit color */

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

/* 24-bit color */

static void color32_to_24(Color32 *src, void *dst, uint64 length);
static void color24_to_32(void *src, Color32 *dst, uint64 length);

const GFX::PixelFmt Color24::format = {
	.bpp  = 24,
	.bypp = 3,
	.mode = GFX::PixelFmt::RGBA,

	.rgba = {
		.r_mask  = {0xff, 0x00, 0x00},		// In reality, r_mask is 4 bytes long, but since this is a 3-byte format we only set the first three. The last byte is automatically set to zero.
		.r_shift = 0,
		.r_size  = 8,

		.g_mask  = {0x00, 0xff, 0x00},
		.g_shift = 8,
		.g_size  = 8,

		.b_mask  = {0x00, 0x00, 0xff},
		.b_shift = 16,
		.b_size  = 8,
	},

	.encode_color32 = color32_to_24,
	.decode_color32 = color24_to_32,
};

void color32_to_24(Color32 *src, void *dst, uint64 length)
{
	uint64 i = 0;
	for (dword *r = (dword *) src, *w = (dword *) dst; i < length; i++)
	{
		*w = *r;	// Copy all 4 bytes of Color32

		r++;
		w = (dword *) &w->bytes[3];	// We move 3 bytes instead of 4 so that we overwrite the alpha byte with the next pixel.
	}
}

void color24_to_32(void *src, Color32 *dst, uint64 length)
{
	uint64 i = 0;
	for (dword *r = (dword *) src, *w = (dword *) dst; i < length; i++)
	{
		*w = *r;
		w->bytes[3] = 0xff;	// We copied junk in the 4th byte as Color24s are only 3 bytes long. Set the 4th (alpha) byte to opaque.

		r = (dword *) &r->bytes[3];
		w++;
	}
}

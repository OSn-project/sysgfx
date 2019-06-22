#include <string.h>
#include "../headers/pixelfmt.h"

using namespace OSn;
using namespace GFX;

/* This is a table used to expand pixel values that	*
 * are smaller than 8-bits into the range of 0-255.	*
 * This solves the problem of a 5-bit white 		*
 * (0b11111) turning into a light-gray (0b11111000)	*
 * when expanded to 8 bits. The arrays are in asce-	*
 * nding order of pixel size. To expand a value,	*
 * do: `PixelFmt::expand_table[val. size]			*
 * [pix. value]`. Original values courtesy of SDL.	*/

static uint8 expand_0b[] = {255};
static uint8 expand_1b[] = {0, 255};
static uint8 expand_2b[] = {0, 85, 170, 255};
static uint8 expand_3b[] = {0, 36, 72, 109, 145, 182, 218, 255};
static uint8 expand_4b[] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
static uint8 expand_5b[] = {0, 8, 16, 24, 32, 41, 49, 57, 65, 74, 82, 90, 98, 106, 115, 123, 131, 139, 148, 156, 164, 172, 180, 189, 197, 205, 213, 222, 230, 238, 246, 255};
static uint8 expand_6b[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 238, 242, 246, 250, 255};
static uint8 expand_7b[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 255};
static uint8 expand_8b[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

uint8 *PixelFmt::expand_table[9] = {
	expand_0b,
	expand_1b,
	expand_2b,
	expand_3b,
	expand_4b,
	expand_5b,
	expand_6b,
	expand_7b,
	expand_8b,
};

Color32 PixelFmt :: decode(dword val, const PixelFmt *in_fmt)
{
	Color32 out;

	/* Extract each color intensity from the pixel value */
	uint8 r_val = be32toh(val & htobe32(in_fmt->rgba.r_mask)) >> (in_fmt->rgba.r_shift);	// We need to convert to system endian so that bit shifts work properly. Also we promissed that the bytes of the channel mask would be interpreted in the order in which theyb are displayed, so we need to convert it to big endian.
	uint8 g_val = be32toh(val & htobe32(in_fmt->rgba.g_mask)) >> (in_fmt->rgba.g_shift);
	uint8 b_val = be32toh(val & htobe32(in_fmt->rgba.b_mask)) >> (in_fmt->rgba.b_shift);
	uint8 a_val = be32toh(val & htobe32(in_fmt->rgba.a_mask)) >> (in_fmt->rgba.a_shift);

	/* Expand the color intensities to the range 0-255 */
	out.red   = PixelFmt::expand_table[in_fmt->rgba.r_size][r_val];
	out.green = PixelFmt::expand_table[in_fmt->rgba.g_size][g_val];
	out.blue  = PixelFmt::expand_table[in_fmt->rgba.b_size][b_val];
	out.alpha = PixelFmt::expand_table[in_fmt->rgba.a_size][a_val];

	if (in_fmt->rgba.a_mask == 0)
		out.alpha = 0xff;				// If the source format does not have an alpha channel, assume that the color is opaque.

	return out;
}

dword PixelFmt :: encode(Color32 col, const PixelFmt *out_fmt)
{
	dword out = 0;

	/* Compose the color intensities in the output format */
	out |= htobe32((col.red   >> (8 - out_fmt->rgba.r_size)) << out_fmt->rgba.r_shift);
	out |= htobe32((col.green >> (8 - out_fmt->rgba.g_size)) << out_fmt->rgba.g_shift);
	out |= htobe32((col.blue  >> (8 - out_fmt->rgba.b_size)) << out_fmt->rgba.b_shift);

	if (out_fmt->rgba.a_mask != 0)
		out |= htobe32((col.alpha >> (8 - out_fmt->rgba.a_size)) << out_fmt->rgba.a_shift);

	return out;
}

dword PixelFmt :: convert(dword in, const PixelFmt *in_fmt, const PixelFmt *out_fmt)
{
	Color32 tmp = PixelFmt::decode(in, in_fmt);
	return PixelFmt::encode(tmp, out_fmt);
}

/*void PixelFmt :: convert(uint8 *in_ptr, const PixelFmt *in_fmt, uint8 *out_ptr, const PixelFmt *out_fmt)
{
	uint32 in_val;
	memcpy(&in_val + (sizeof(uint32) - in_fmt->bypp), in_ptr, in_fmt->bypp);

	uint32 out = PixelFmt::convert(in_val, in_fmt, out_fmt);
	memcpy(out_ptr, &out + (sizeof(uint32) - in_fmt->bypp), out_fmt->bypp);
}*/

bool PixelFmt :: compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b)
{
	return memcmp(fmt_a, fmt_b, sizeof(PixelFmt)) == 0;
}

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <osndef.h>
#include <byteswap.h>
#include <base/misc.h>

#include "../headers/pixelfmt.h"

using namespace OSn;
using namespace GFX;

/* This is a table used to expand pixel values that	*
 * are smaller than 8-bits into the range of 0-255.	*
 * This solves the problem of a 5-bit white 		*
 * (0b11111) turning into a light-gray (0b11111000)	*
 * when expanded to 8 bits. The arrays are in asce-	*
 * nding order of pixel size. To expand a value,	*
 * do: `PixelFmt::expand_table[size in bits]		*
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

/* Reference counting */
#include <stdio.h>
PixelFmt *PixelFmt :: ref(PixelFmt *fmt)
{
	fmt->_refcount++;
	printf("pixelfmt %p referenced. _refcount = %d\n", fmt, fmt->_refcount);
	return fmt;
}

PixelFmt *PixelFmt :: unref(PixelFmt *fmt)
{printf("pixelfmt %p unref.", fmt);
	if (fmt->_refcount == 0)	// This check needs to come first to prevent the integer from underflowing.
	{printf("\tfreed\n", fmt);
		if (fmt->_malloced)
		{
			if (fmt->mode == PixelFmt::INDEXED) free(fmt->palette.colors);
			free(fmt);
		}
	}
	else	// If there still *are* references to the struct AND it hasn't been freed.
	{
		fmt->_refcount--;printf("\t_refcount = %d\n", fmt->_refcount);
	}

	return fmt;
}

/* */

static void bswap_3bytes(uint8 *i)
{
	/* Swap the bytes of the referenced 24-bit integer */

	uint8 tmp = i[0];
	i[0] = i[2];
	i[2] = tmp;
}

inline void swap_bytes(uint8 *i, uint8 size)
{
	switch (size)
	{
		case 2:  *(uint16 *) i = bswap_16(*(uint16 *) i); return;
		case 3:  bswap_3bytes(i); return;
		case 4:  *(uint32 *) i = bswap_32(*(uint32 *) i); return;
		default: return;
	}
}

Color32 PixelFmt :: decode(dword val, const PixelFmt *in_fmt)
{
	assert(in_fmt->mode == PixelFmt::RGBA);

	/* The code below may accidentally read one byte beyond the dword if a colour 		*
	 * channel is only present in the last byte. (`bytes[in_fmt->rgba.r_shift / 8 + 1]` *
	 * gets the second byte that may contain the channel, but this may be out of bounds *
	 * if the whole channel is contained in the fourth byte.)							*
	 * See below:																		*
	 *
	 * |xxxxxxxx|xxxxxx12|345xxxxx|xxxxxxxx|
	 *           ^^^^^^^^ ^^^^^^^^
	 *
	 * |xxxxxxxx|xxxxxxxx|xxxxxxxx|xxx12345|
	 *                             ^^^^^^^^ ^^^^^^^^
	 *
	 * So we need to own the extra byte as well unless we want to complicate our code.	*
	 * We do this by storing the value in the lowest-in-memory part of a `qword`.		*/

	Color32 out;	// decoded colour intensities

	qword padded = 0;
	*((dword *) &padded) = val;

	uint8 *bytes = (uint8 *) &padded;

	/* First swap the bytes if necessary */
	if (in_fmt->rgba.byte_swap)
	{
		swap_bytes(bytes, in_fmt->bypp);
	}

	/* A step-by-step explanation of the following code.
	 * Assumes values < 8 bytes in size.
	 * Note: `in_fmt->rgba.r_shift / 8` gets us the index of the first byte containing part of the channel.

		uint8 r_val = 	(bytes[in_fmt->rgba.r_shift / 8]	// First byte containing the channel we want. The relevant bits will be to the right (least sig.).
						| in_fmt->rgba.r_mask[in_fmt->rgba.r_shift / 8]		// Mask out only the relevant bits. Mask provided in format definition.
						<< in_fmt->rgba.r_shift % 8) &	// Shift them to make space for the bits from the following byte.
						(bytes[in_fmt->rgba.r_shift / 8 + 1]	// Get the second byte containing the channel we want. The relevant bits will be to the left (most sig.).
						| in_fmt->rgba.r_mask[in_fmt->rgba.r_shift / 8 + 1]	// Mask out the relevant bits.
						>> 8 - (in_fmt->rgba.r_shift % 8))	// Shift to fill the remaining space in the output.
	 */

	/* Extract each color intensity from the pixel value */
	uint8 r_val = (	(bytes[in_fmt->rgba.r_shift / 8]     & in_fmt->rgba.r_mask[in_fmt->rgba.r_shift / 8])     << (    in_fmt->rgba.r_shift % 8) |
					(bytes[in_fmt->rgba.r_shift / 8 + 1] & in_fmt->rgba.r_mask[in_fmt->rgba.r_shift / 8 + 1]) >> (8 - in_fmt->rgba.r_shift % 8)) >> (8 - in_fmt->rgba.r_size);
	uint8 g_val = (	(bytes[in_fmt->rgba.g_shift / 8]     & in_fmt->rgba.g_mask[in_fmt->rgba.g_shift / 8])     << (    in_fmt->rgba.g_shift % 8) |
					(bytes[in_fmt->rgba.g_shift / 8 + 1] & in_fmt->rgba.g_mask[in_fmt->rgba.g_shift / 8 + 1]) >> (8 - in_fmt->rgba.g_shift % 8)) >> (8 - in_fmt->rgba.g_size);
	uint8 b_val = (	(bytes[in_fmt->rgba.b_shift / 8]     & in_fmt->rgba.b_mask[in_fmt->rgba.b_shift / 8])     << (    in_fmt->rgba.b_shift % 8) |
					(bytes[in_fmt->rgba.b_shift / 8 + 1] & in_fmt->rgba.b_mask[in_fmt->rgba.b_shift / 8 + 1]) >> (8 - in_fmt->rgba.b_shift % 8)) >> (8 - in_fmt->rgba.b_size);
	uint8 a_val = (	(bytes[in_fmt->rgba.a_shift / 8]     & in_fmt->rgba.a_mask[in_fmt->rgba.a_shift / 8])     << (    in_fmt->rgba.a_shift % 8) |
					(bytes[in_fmt->rgba.a_shift / 8 + 1] & in_fmt->rgba.a_mask[in_fmt->rgba.a_shift / 8 + 1]) >> (8 - in_fmt->rgba.a_shift % 8)) >> (8 - in_fmt->rgba.a_size);

	/* Expand the color intensities to the range 0-255 */
	out.red   = PixelFmt::expand_table[in_fmt->rgba.r_size][r_val];
	out.green = PixelFmt::expand_table[in_fmt->rgba.g_size][g_val];
	out.blue  = PixelFmt::expand_table[in_fmt->rgba.b_size][b_val];
	out.alpha = PixelFmt::expand_table[in_fmt->rgba.a_size][a_val];

	if (in_fmt->rgba.invert_alpha)
		out.alpha = 255 - out.alpha;

	if (in_fmt->rgba.a_size == 0)
		out.alpha = 255;				// If the source format does not have an alpha channel, assume that the color is opaque.

	return out;
}

dword PixelFmt :: encode(Color32 col, const PixelFmt *out_fmt)
{
	assert(out_fmt->mode == PixelFmt::RGBA);

	union {
		uint8 bytes[4];
		dword val;
	} out;

	out.val = 0;

	/* Invert alpha if necessary */
	if (out_fmt->rgba.invert_alpha)
		col.alpha = 255 - col.alpha;

	/* Squash the color values to the sizes required by the format	*
	 * by erasing their least significant bits.						*/

	col.red   &= 0xff << (8 - out_fmt->rgba.r_size);
	col.green &= 0xff << (8 - out_fmt->rgba.g_size);
	col.blue  &= 0xff << (8 - out_fmt->rgba.b_size);
	col.alpha &= 0xff << (8 - out_fmt->rgba.a_size);

	/* Compose the color intensities in the output format */

	out.bytes[out_fmt->rgba.r_shift / 8]     |= col.red   >> (    out_fmt->rgba.r_shift % 8);		// Split the byte into two parts -- one to go into the lower-in-memory byte of the color value, and one to go into the higher one.
	out.bytes[out_fmt->rgba.r_shift / 8 + 1] |= col.red   << (8 - out_fmt->rgba.r_shift % 8);		// They'll form the right-most bits of the first byte and the left-most bits of the second byte.
	out.bytes[out_fmt->rgba.g_shift / 8]     |= col.green >> (    out_fmt->rgba.g_shift % 8);
	out.bytes[out_fmt->rgba.g_shift / 8 + 1] |= col.green << (8 - out_fmt->rgba.g_shift % 8);
	out.bytes[out_fmt->rgba.b_shift / 8]     |= col.blue  >> (    out_fmt->rgba.b_shift % 8);
	out.bytes[out_fmt->rgba.b_shift / 8 + 1] |= col.blue  << (8 - out_fmt->rgba.b_shift % 8);

	if (out_fmt->rgba.a_size != 0)	// Only write the alpha channel if the format supports it.
	{
		out.bytes[out_fmt->rgba.a_shift / 8]     |= col.alpha >> (    out_fmt->rgba.a_shift % 8);	// TODO: Could & with the bitmask to be 100% certain that only the designated bits are set.
		out.bytes[out_fmt->rgba.a_shift / 8 + 1] |= col.alpha << (8 - out_fmt->rgba.a_shift % 8);
	}

	/* Swap the bytes if format is little-endian */
	if (out_fmt->rgba.byte_swap)
	{
		swap_bytes(out.bytes, out_fmt->bypp);
	}

	return out.val;
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

//owning PixelFmt *PixelFmt :: copy(const PixelFmt *fmt)
//{
//	PixelFmt *copy = (PixelFmt *) malloc(sizeof(PixelFmt));
//
//	copy->mode = fmt->mode;
//
//	copy->bpp  = fmt->bpp;
//	copy->bypp = fmt->bypp;
//
//	switch (fmt->mode)
//	{
//	case PixelFmt::RGBA:
//		memcpy(&copy->rgba, &fmt->rgba, sizeof(RGBADef));
//		break;
//
//	case PixelFmt::INDEXED:
//		copy->palette.size   = fmt->palette.size;
//		copy->palette.colors = (Color32 *) memdup(fmt->palette.colors, fmt->palette.size * sizeof(Color32));
//		break;
//	}
//
//	return copy;
//}

bool PixelFmt :: compare(const PixelFmt *fmt_a, const PixelFmt *fmt_b)
{
	return memcmp(fmt_a, fmt_b, sizeof(PixelFmt)) == 0;
}


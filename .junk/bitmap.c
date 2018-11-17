#include "bitmap.h"

#include <stdlib.h>
#include <base/misc.h>


const OSn_PixelInfo __bmp32_format = {
	.bpp = 32,
	0xff000000, 24,  0,
	0x00ff0000, 16,  0,
	0x0000ff00,  8,  0,
	0x000000ff,  0,  0
};

OSn_Bitmap *bmp_new(const OSn_PixelInfo *fmt, uint32 width, uint32 height)
{
	OSn_Bitmap *bmp = malloc(sizeof(OSn_Bitmap));

	if (bmp == NULL) return NULL;

	bmp_init(bmp, fmt, width, height, NULL);

	return bmp;
}

OSn_Bitmap *bmp32_new(uint32 width, uint32 height)
{
	OSn_Bitmap *bmp = malloc(sizeof(OSn_Bitmap));

	if (bmp == NULL) return NULL;

	bmp_init(bmp, &__bmp32_format, width, height, NULL);

	return bmp;
}

void bmp_init(OSn_Bitmap *bmp, const OSn_PixelInfo *fmt, uint32 width, uint32 height, uint8 *data)
{
	/* Initialize a bitmap with the provided information */
	
	bmp->width  = width;
	bmp->height = height;
	bmp->pitch  = width;
	
	bmp->fmt_info = info;
	
	bmp->flags = 0;
	
	if (data != NULL)
	{
		/* If they've provided memory to hold the image data */
		bmp->data = data;
		bmp->flags &= ~BMP_OWNING;		// Don't free the image data upon destruction
	}
	else
	{
		/* To calculate the amount of memory we need to allocate,			*
		 * we multiply the width by the height to get the number of pixels,	*
		 * then by the number of bits per pixel to get the number of bits 	*
		 * needed, and then divide by 8 (using ceiling division to account	*
		 * for an incomplete last byte) to get the number of bytes.			*/
		
		bmp->data = (uint8 *) malloc(b_ceil(height * bmp->pitch * bmp->fmt_info->bpp, 8));
		bmp->flags |= BMP_OWNING;
	}
}

void bmp_free(OSn_Bitmap *bmp)
{
	if (bmp == NULL)
	{
		return;
	}
	
	if (bmp->flags & BMP_OWNING)
	{
		free(bmp->data);
	}
	
	free(bmp);
}

//~ void *bmp_pixel_at(OSn_Bitmap *bmp, int32 x, int32 y)
//~ {
	//~ if ((x < 0 || x > bmp->width) ||	/* Check that the pixel isn't	*/
		//~ (y < 0 || y > bmp->height))		/* out of bounds.				*/
	//~ {
		//~ return NULL;
	//~ }

	//~ void *pixel = bmp->data[(y * bmp->width + x) * bmp->format->pixel_size];

	//~ return pixel;
//~ }

//~ void bmp_set_color32(OSn_Bitmap *bmp, int32 x, int32 y, int32 color32)
//~ {
	//~ void *pixel = nbmp_pixel_at(bmp, x, y);

	//~ switch (bmp->format.pixel_size)
	//~ {
		//~ /* The only thing that changes with the pixel size is the	*
		 //~ * size of the integer that we cast the pointer as.			*/

		//~ case 1:
		//~ {
			//~ *((uint8 *) pixel) =	  (((color32 & 0xff000000) >> 24) >> bmp->format.r_loss) << bmp->format.r_shift)
									//~ | (((color32 & 0x00ff0000) >> 16) >> bmp->format.g_loss) << bmp->format.g_shift)
									//~ | (((color32 & 0x0000ff00) >>  8) >> bmp->format.b_loss) << bmp->format.b_shift)
									//~ | (((color32 & 0x000000ff) >>  0) >> bmp->format.a_loss) << bmp->format.a_shift);

			//~ break;
		//~ }
		//~ case 2:
		//~ {
			//~ *((uint16 *) pixel) =	  (((color32 & 0xff000000) >> 24) >> bmp->format.r_loss) << bmp->format.r_shift)
									//~ | (((color32 & 0x00ff0000) >> 16) >> bmp->format.g_loss) << bmp->format.g_shift)
									//~ | (((color32 & 0x0000ff00) >>  8) >> bmp->format.b_loss) << bmp->format.b_shift)
									//~ | (((color32 & 0x000000ff) >>  0) >> bmp->format.a_loss) << bmp->format.a_shift);

			//~ break;
		//~ }
		//~ /* case 3; for 24-bit images */
		//~ case 4:
		//~ {
			//~ *((uint32 *) pixel) =	  (((color32 & 0xff000000) >> 24) >> bmp->format.r_loss) << bmp->format.r_shift)
									//~ | (((color32 & 0x00ff0000) >> 16) >> bmp->format.g_loss) << bmp->format.g_shift)
									//~ | (((color32 & 0x0000ff00) >>  8) >> bmp->format.b_loss) << bmp->format.b_shift)
									//~ | (((color32 & 0x000000ff) >>  0) >> bmp->format.a_loss) << bmp->format.a_shift);

			//~ break;
		//~ }
	//~ }

	//~ pixel = ((OSn_Color32) color32);
//~ }

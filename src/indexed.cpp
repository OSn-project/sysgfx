#include <float.h>
#include <math.h>

#include "indexed.h"

using namespace OSn;
using namespace OSn::GFX;

inline int sq(int i) { return i * i; }

uint8 closest_color(Color32 orig, PixelFmt *fmt)		// Hey boi look 'ere: https://www.bisqwit.iki.fi/story/howto/dither/jy/
{														// And 'ere:          https://github.com/wjaguar/mtPaint/blob/960cff9ac0ec0834ea358fff49818125b9309d86/src/memory.c#L3016
	/* Get the index of the color in the format's palette	*
	 * that is the most similar to the specified colour.	*
	 * Could be improved by converting to a colourspace		*
	 * that closer represents human vision (like HSV)		*
	 * and finding the closest colour in that.				*/

	uint16 closest_idx  = 0;
	float  closest_loss = FLT_MAX;

	for (uint16 i = 0; i < fmt->palette.size; i++)
	{
		Color32 col = fmt->palette.colors[i];
		float  loss = sqrt(sq(orig.red - col.red) + sq(orig.green - col.green) + sq(orig.blue - col.blue));		// Sorry no alpha

		if (loss < closest_loss)
		{
			closest_idx = i;
			closest_loss = loss;
		}
	}

	return closest_idx;
}

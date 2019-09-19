#ifndef OSn_RECT_H_
#define OSn_RECT_H_

#include <osndef.h>

namespace OSn
{
	struct Rect
	{
		int16 x, y;
		int16 w, h;

		bool intersects(const Rect *rect) const;
		Rect intersection(const Rect *rect) const;

		void center(const Rect *inside);
		static Rect *clip(Rect *rect, const Rect *bounds);
		static Rect clipped(const Rect *rect, const Rect *bounds);

		/* Eyecandy for treating as 2nd x,y coordinates */
		inline int16 x1() const { return x;     }	// Just eyecandy
		inline int16 y1() const { return y;     }
		inline int16 x2() const { return x + w; }
		inline int16 y2() const { return y + h; }

		inline void x1y1(int16 x1, int16 y1) { x = x1    ; y = y1    ; }
		inline void x2y2(int16 x2, int16 y2) { w = x2 - x; h = y2 - y; }
		inline void wh(int16 w, int16 h) { this->w = w; this->h = h; }
	};
}

#endif

#ifndef OSn_GFX_INDEXED_H_
#define OSn_GFX_INDEXED_H_

#include "../headers/color.h"
#include "../headers/pixelfmt.h"

using namespace OSn;

uint8 closest_color(Color32 orig, GFX::PixelFmt *fmt);

#endif

#pragma once

#include "canvas.h"
#include "color/color.h"

class Circle
{
public:
    static bool draw(Canvas* canvas, int cx, int cy, int r, Color color);
    static bool draw_midpoint(Canvas* canvas, int cx, int cy, int r, Color color);
    static bool draw_filled(Canvas* canvas, int cx, int cy, int r, Color color);

    // static bool getCircle(float x1, float y1, float x2, float y2, float x3, float y3, float* cx, float* cy, float* r);
    // static bool getCircle(float x1, float y1, float x2, float y2, float x3, float y3, float* cx, float* cy, float* r);
};

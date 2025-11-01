#pragma once

#include "canvas.h"
#include "color/color.h"

class BezierCurve
{
public:
    static void draw_quadratic(Canvas* canvas, int x1, int y1, int xc, int yc, int x2, int y2, float resolution, Color color);
    static void draw_cubic(Canvas* canvas, int x1, int y1, int x1c, int y1c, int x2c, int y2c, int x2, int y2, float resolution, Color color);
};

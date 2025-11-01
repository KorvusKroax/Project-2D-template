#pragma once

#include "canvas.h"
#include "color/color.h"

class Triangle
{
public:
    static void draw(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, Color color, int pattern = 0xffffffff);
    static void draw_filled(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, Color color);
    static void draw_filled_withBias(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, Color color);

private:
    static inline int cross(int x1, int y1, int x2, int y2, int px, int py)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
        // n > 0: p is left
        // n = 0: p is on line
        // n < 0: p is right
    }
};

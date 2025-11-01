#pragma once

#include "canvas.h"
#include "color/color.h"

class Rectangle
{
public:
    static bool draw(Canvas* canvas, int x, int y, int width, int height, Color color, int pattern = 0xffffffff);
    static bool draw_filled(Canvas* canvas, int x, int y, int width, int height, Color color);
};

#pragma once

#include "canvas.h"
#include "color/color.h"

class Fill
{
public:
    static bool flood(Canvas* canvas, int x, int y, Color color);
    static bool span(Canvas* canvas, int x, int y, Color color);
};

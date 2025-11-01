#pragma once

#include "canvas.h"
#include "rectangle.h"

# include <iostream>

class Widget
{
public:
    float x, y, width, height;
    bool hovered = false;

    Widget(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height)
    {}

    virtual void draw(Canvas* canvas)
    {
        Rectangle::draw(canvas, this->x, this->y, this->width, this->height, EGA_WHITE, 0xfefefefe);
    }

    virtual void updateHover(float mx, float my)
    {
        this->hovered = mx >= this->x && mx <= this->x + this->width && my >= this->y && my <= this->y + this->height;
    }

    virtual void onClick()
    {}
};

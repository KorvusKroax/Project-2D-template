#pragma once

#include "canvas.h"

#include <functional> // function

class Widget
{
public:
    float x, y, width, height;
    bool hovered = false;
    std::function<void()> onClickCallback;

    Widget(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height)
    {}

    virtual void draw(Canvas* canvas);
    virtual void updateHover(float mx, float my);
    virtual void onClick();
};

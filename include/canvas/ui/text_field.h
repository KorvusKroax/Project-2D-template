#pragma once

#include "widget.h"

#include "canvas.h"
#include "text.h"

class TextField : public Widget
{
public:
    std::string text;
    Text::Options opts;

    TextField(float x, float y, float width, float height, const std::string& text, const Text::Options& opts)
        : Widget(x, y, width, height), text(text), opts(opts)
    {}

    void draw(Canvas* canvas) override
    {
        Text::draw(canvas, this->x, this->y, this->text, this->opts);
    }
};

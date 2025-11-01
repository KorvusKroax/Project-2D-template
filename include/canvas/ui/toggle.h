#pragma once

#include "widget.h"
#include "text.h"

#include "canvas.h"
#include "rectangle.h"

#include <functional> // function
#include <cmath> // lround

class Toggle : public Widget
{
public:
    std::string label;
    Text::Options opts;
    std::function<void()> onClickCallback;

    Color color = EGA_DARK_GRAY;

    bool checked = false;

    Toggle(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
        : Widget(x, y, width, height), label(label), opts(opts)
    {}

    void draw(Canvas* canvas) override
    {
        Rectangle::draw(canvas, this->x, this->y, this->width, this->height, this->color);

        if (this->checked) {
            Rectangle::draw_filled(canvas, this->x + 2, this->y + 2, this->width - 4, this->height - 4, this->color);
        }

        Text::draw(canvas,
            this->x + this->width + 2,
            this->y + (int)std::lround((this->height - Text::calcLineHeight(this->opts)) * .5f),
            this->label, this->opts
        );
    }

    void updateHover(float mx, float my) override
    {
        Widget::updateHover(mx, my);
        this->color = this->hovered ? EGA_LIGHT_GRAY : EGA_DARK_GRAY;
    }

    void onClick() override
    {
        this->checked = !this->checked;
        if (onClickCallback) onClickCallback();
    }
};

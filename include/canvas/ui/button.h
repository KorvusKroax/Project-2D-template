#pragma once

#include "widget.h"
#include "text_field.h"

#include "canvas.h"
#include "rectangle.h"

#include <functional>

class Button : public Widget
{
public:
    TextField textField;
    std::function<void()> onClickCallback;

    Color color = EGA_DARK_GRAY;

    Button(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
        : Widget(x, y, width, height), textField(TextField(x, y, width, height, label, opts))
    {}

    void draw(Canvas* canvas) override
    {
        Rectangle::draw_filled(canvas, this->x, this->y, this->width, this->height, this->color);
        textField.draw(canvas);
    }

    void updateHover(float mx, float my) override
    {
        Widget::updateHover(mx, my);
        this->color = this->hovered ? EGA_LIGHT_GRAY : EGA_DARK_GRAY;
    }

    void onClick() override
    {
        if (onClickCallback) onClickCallback();
    }
};

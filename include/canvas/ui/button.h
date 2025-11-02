#pragma once

#include "ui/widget.h"
#include "ui/text_field.h"

#include "canvas.h"
#include "color/color.h"

class Button : public Widget
{
public:
    TextField textField;

    Color color = EGA_DARK_GRAY;

    Button(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
        : Widget(x, y, width, height), textField(TextField(x, y, width, height, label, opts))
    {}

    void draw(Canvas* canvas) override;
    void updateHover(float mx, float my) override;
};

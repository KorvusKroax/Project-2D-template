#pragma once

#include "ui/widget.h"
#include "ui/text_field.h"

#include "canvas.h"
#include "color/color.h"

class Toggle : public Widget
{
public:
    TextField textField;

    Color color = EGA_DARK_GRAY;

    bool checked = false;

    Toggle(float x, float y, float width, float height, const std::string& label, const Text::Options& opts);

    void draw(Canvas* canvas) override;
    void updateHover(float mx, float my) override;
    void onClick() override;
};

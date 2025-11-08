#pragma once

#include "ui/widget.h"
#include "ui/text_field.h"

#include "canvas.h"
#include "color/color.h"

class Button : public Widget
{
public:
    Canvas* image = nullptr;
    Color color = DARK_GRAY;

    TextField textField;

    Button(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
        : Widget(x, y, width, height), textField(TextField(x, y, width, height, label, opts))
    {}

    Button(float x, float y, Canvas* image, const std::string& label, const Text::Options& opts)
        : Widget(x, y, image->width, image->height), image(image), textField(TextField(x, y, image->width, image->height, label, opts))
    {}

    void draw(Canvas* canvas) override;
};

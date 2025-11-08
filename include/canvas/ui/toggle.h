#pragma once

#include "ui/widget.h"
#include "ui/text_field.h"

#include "canvas.h"
#include "color/color.h"

class Toggle : public Widget
{
public:
    Canvas* image = nullptr;
    Canvas* checkImage = nullptr;
    Color color = DARK_GRAY;
    TextField textField;

    bool checked = false;

    Toggle(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
        : Widget(x, y, width, height), textField(TextField(x, y, Text::calcLineWidth(Text::utf8ToCodepoints(label), opts), height, label, opts))
    {}

    Toggle(float x, float y, Canvas* image, Canvas* checkImage, const std::string& label, const Text::Options& opts)
        : Widget(x, y, image->width, image->height), image(image), checkImage(checkImage), textField(TextField(x, y, Text::calcLineWidth(Text::utf8ToCodepoints(label), opts), image->height, label, opts))
    {}

    void draw(Canvas* canvas) override;
    void onClick() override;
};

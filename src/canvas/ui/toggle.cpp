#include "ui/toggle.h"

#include "draw/rectangle.h"

#include <cmath> // lround

Toggle::Toggle(float x, float y, float width, float height, const std::string& label, const Text::Options& opts)
    : Widget(x, y, width, height), textField(TextField(x, y, Text::calcLineWidth(Text::utf8ToCodepoints(label), opts), height, label, opts))
{}

void Toggle::draw(Canvas* canvas)
{
    this->color = this->clicked && this->hovered ? EGA_RED : (this->hovered ? EGA_LIGHT_GRAY : EGA_DARK_GRAY);

    Rectangle::draw(canvas, this->x, this->y, this->width, this->height, this->color);
    if (this->checked) {
        Rectangle::draw_filled(canvas, this->x + 2, this->y + 2, this->width - 4, this->height - 4, this->color);
    }

    this->textField.x = this->x + this->width + 2;
    this->textField.y = this->y + (int)std::lround((this->height - Text::calcLineHeight(this->textField.opts)) * .5f);

    this->textField.draw(canvas);
}

void Toggle::onClick()
{
    Widget::onClick();
    if (this->hovered && this->clicked) this->checked = !this->checked;
}

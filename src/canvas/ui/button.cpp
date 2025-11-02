#include "ui/button.h"

#include "draw/rectangle.h"

void Button::draw(Canvas* canvas)
{
    Rectangle::draw_filled(canvas, this->x, this->y, this->width, this->height, this->color);
    this->textField.draw(canvas);
}

void Button::updateHover(float mx, float my)
{
    Widget::updateHover(mx, my);
    this->color = this->hovered ? EGA_LIGHT_GRAY : EGA_DARK_GRAY;
}

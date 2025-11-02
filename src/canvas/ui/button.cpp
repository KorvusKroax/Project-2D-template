#include "ui/button.h"

#include "draw/rectangle.h"

void Button::draw(Canvas* canvas)
{
    this->color = this->clicked && this->hovered ? EGA_RED : (this->hovered ? EGA_LIGHT_GRAY : EGA_DARK_GRAY);

    Rectangle::draw_filled(canvas, this->x, this->y, this->width, this->height, this->color);

    this->textField.x = this->x;
    this->textField.y = this->y;

    this->textField.draw(canvas);
}

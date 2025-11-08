#include "ui/button.h"

#include "draw/rectangle.h"

void Button::draw(Canvas* canvas)
{
    this->color = this->clicked && this->hovered ? RED : (this->hovered ? LIGHT_GRAY : DARK_GRAY);

    if (this->image) {
        canvas->setPixels_blend(this->x, this->y, this->image);
    } else {
        Rectangle::draw_filled(canvas, this->x, this->y, this->width, this->height, this->color);
    }

    this->textField.x = this->x;
    this->textField.y = this->y;
    this->textField.textAlign = CENTER_CENTER;
    this->textField.draw(canvas);
}

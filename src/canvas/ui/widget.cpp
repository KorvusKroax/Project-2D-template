#include "ui/widget.h"

#include "draw/rectangle.h"

void Widget::draw(Canvas* canvas)
{
    Rectangle::draw(canvas, this->x, this->y, this->width, this->height, EGA_WHITE, 0xfefefefe);
}

void Widget::updateHover(float mx, float my)
{
    this->hovered = mx >= this->x && mx <= this->x + this->width && my >= this->y && my <= this->y + this->height;
}

void Widget::onClick()
{
    if (onClickCallback) onClickCallback();
}

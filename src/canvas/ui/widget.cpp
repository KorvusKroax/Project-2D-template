#include "ui/widget.h"

#include "draw/rectangle.h"

void Widget::draw(Canvas* canvas)
{
    Rectangle::draw(canvas, this->x, this->y, this->width, this->height, EGA_WHITE, 0xfefefefe);
}

void Widget::updateHover(float mx, float my)
{
    this->hovered = mx >= this->x && mx <= this->x + this->width && my >= this->y && my <= this->y + this->height;
    this->clicked &= this->hovered;
}

void Widget::onClick()
{
    this->clicked = true;
    if (this->onClick_callback) this->onClick_callback();
}

void Widget::onDrag()
{
    if (this->clicked && this->hovered && this->onDrag_callback) this->onDrag_callback();
}

void Widget::onRelease()
{
    this->clicked = false;
    if (this->onRelease_callback) this->onRelease_callback();
}

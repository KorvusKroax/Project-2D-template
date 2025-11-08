#include "ui/toggle.h"

#include "draw/rectangle.h"

void Toggle::draw(Canvas* canvas)
{
    this->color = this->clicked && this->hovered ? RED : (this->hovered ? LIGHT_GRAY : DARK_GRAY);

    if (this->image) {
        canvas->setPixels_blend(this->x, this->y, this->image);
    } else {
        Rectangle::draw(canvas, this->x, this->y, this->width, this->height, this->color);
    }

    if (this->checked) {
        if (this->checkImage) {
            canvas->setPixels_blend(this->x, this->y, this->checkImage);
        } else {
            Rectangle::draw_filled(canvas, this->x + 2, this->y + 2, this->width - 4, this->height - 4, this->color);
        }
    }

    this->textField.x = this->x + this->width + 2;
    this->textField.y = this->y;
    this->textField.textAlign = LEFT_CENTER;
    this->textField.draw(canvas);
}

void Toggle::onClick()
{
    Widget::onClick();
    if (this->hovered && this->clicked) this->checked = !this->checked;
}

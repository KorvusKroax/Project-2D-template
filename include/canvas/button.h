#pragma once

#include "canvas.h"
#include "open_gl.h"

#include "text/text_field.h"

#include "color/color.h"
#include "draw/rectangle.h"

struct Button
{
    struct Options {
        int x = 10;
        int y = 10;
        int width = 100;
        int height = 25;

        Color bgColor = Color(127, 127, 127);
        Color bgColor_hover = Color(191, 191, 191);
        Color bgColor_clicked = Color(200, 0, 0);

        // Canvas* bgImage = nullptr;
        // Canvas* bgImage_hover = nullptr;
        // Canvas* bgImage_clicked = nullptr;

        Color textColor = Color(0, 0, 0);
        Color textColor_hover = Color(0, 0, 0);
        Color textColor_clicked = Color(255, 255, 255);

        std::string text = "";
        TextAlign textAlign = CENTER_CENTER;
        Font* font = nullptr;

        bool operator != (const Options& other) const {
            return
                this->x != other.x ||
                this->y != other.y ||
                this->width != other.width ||
                this->height != other.height ||

                this->bgColor != other.bgColor ||
                this->bgColor_hover != other.bgColor_hover ||
                this->bgColor_clicked != other.bgColor_clicked ||

                // this->bgImage != other.bgImage ||
                // this->bgImage_hover != other.bgImage_hover ||
                // this->bgImage_clicked != other.bgImage_clicked ||

                this->textColor != other.textColor ||
                this->textColor_hover != other.textColor_hover ||
                this->textColor_clicked != other.textColor_clicked ||

                this->text != other.text ||
                this->textAlign != other.textAlign ||
                this->font != other.font;
        }
    };

    enum Status {
        NOPE,
        HOVER,
        CLICKED
    };

    Options opts;
    Status status = NOPE;

    TextField::Options textFieldOpts;
    TextField textField;

    explicit Button(const Options &initialOpts)
    {
        this->opts = initialOpts;

        this->textFieldOpts = {
            .width = this->opts.width,
            .height = this->opts.height,

            .textAlign = this->opts.textAlign,
            .text = this->opts.text,
            .textOpts = {
                .font = this->opts.font,
                .textColor = this->opts.textColor
            },
        };

        this->textField = TextField(this->textFieldOpts);
    }

    void draw(OpenGL* openGL, Options* newOpts = nullptr)
    {
        if (newOpts) {
            if (this->opts != *newOpts) this->opts = *newOpts;
        }

        update(openGL);

        Color bgColor;
        switch (this->status) {
            case CLICKED:
                bgColor = this->opts.bgColor_clicked;
                this->textFieldOpts.textOpts.textColor = this->opts.textColor_clicked;
                break;
            case HOVER:
                bgColor = this->opts.bgColor_hover;
                this->textFieldOpts.textOpts.textColor = this->opts.textColor_hover;
                break;
            default: // NOPE
                bgColor = this->opts.bgColor;
                this->textFieldOpts.textOpts.textColor = this->opts.textColor;
                break;
        }

        Rectangle::draw_filled(openGL->canvas, this->opts.x, this->opts.y, this->opts.width, this->opts.height, bgColor);

        if (this->textFieldOpts.text.size()) {
            this->textField.draw(openGL->canvas, this->opts.x, this->opts.y, &this->textFieldOpts);
        }
    }

    void update(OpenGL* openGL)
    {
        int mx, my;
        openGL->getMousePosition(&mx, &my);

        this->status =
            mx < this->opts.x || mx > this->opts.x + this->opts.width || my < this->opts.y || my > this->opts.y + this->opts.height ? NOPE :
                (glfwGetMouseButton(openGL->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? CLICKED : HOVER);
    }
};

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
        Color bgColor = CLEAR;
        // Canvas* bgImage = nullptr;

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
                // this->bgImage != other.bgImage ||

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
    Status status;

    TextField::Options textFieldOptions;
    TextField textField;

    explicit Button(const Options &initialOpts)
    {
        this->opts = initialOpts;

        this->textFieldOptions = {
            .width = this->opts.width,
            .height = this->opts.height,

            .textAlign = this->opts.textAlign,
            .text = this->opts.text,
            .textOptions = {
                .font = this->opts.font,
            },
        };

        this->textField = TextField(this->textFieldOptions);
    }

    void draw(Canvas* canvas, Options* newOpts = nullptr)
    {
        if (newOpts) {
            if (this->opts != *newOpts) this->opts = *newOpts;
        }

        Rectangle::draw_filled(canvas, this->opts.x, this->opts.y, this->opts.width, this->opts.height, this->opts.bgColor);
        Rectangle::draw(canvas, this->opts.x, this->opts.y, this->opts.width, this->opts.height, EGA_RED);

        if (this->textFieldOptions.text.size()) {
            this->textField.draw(canvas, this->opts.x, this->opts.y, &this->textFieldOptions);
        }
    }

    void update(OpenGL* openGL)
    {
        int mx, my;
        openGL->getMousePosition(&mx, &my);

        this->status =
            mx < opts.x || mx > opts.x + opts.width || my < opts.y || my > opts.y + opts.height ? NOPE :
                (glfwGetMouseButton(openGL->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? CLICKED : HOVER);
    }
};

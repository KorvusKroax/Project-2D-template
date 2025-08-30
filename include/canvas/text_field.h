#pragma once

#include "canvas.h"
#include "text.h"
#include "font.h"

#include <iostream>

struct TextField
{
    Font* font;
    std::string* lines;
    int lineCount;

    int width, height;

    TextField(const char* text, Font* font)
    {
        init(text, font);
    }

    ~TextField() { delete[] this->lines; }

    void init(const char* text, Font* font = nullptr)
    {
        if (font != nullptr) this->font = font;
        this->lineCount = 1;

        int i = 0;
        while (text[i] != '\0')
            if (text[i++] == '\n') this->lineCount++;
        this->lines = new std::string[this->lineCount];

        int maxColCount = 0;
        if (this->lineCount > 1) {
            int i = 0;
            int colCount = 0;
            int rowCount = 0;
            std::string line = "";
            while (text[i] != '\0') {
                if (text[i] == '\n') {
                    this->lines[rowCount++] = line;
                    maxColCount = std::max(maxColCount, colCount);
                    colCount = 0;
                    line = "";
                } else {
                    colCount++;
                    line += text[i];
                }
                i++;
            }
            this->lines[rowCount++] = line;
            maxColCount = std::max(maxColCount, colCount);
        } else {
            this->lines[0] = text;
            maxColCount = strlen(text);
        }

        this->width = this->font->charWidth * maxColCount;
        this->height = this->font->charHeight * this->lineCount;
    }

    bool draw(Canvas* canvas, int x, int y, Color textColor)
    {
        if (x + this->width < 0 || x >= canvas->width || y + this->height < 0 || y >= canvas->height) return false;

        for (int i = 0; i < this->lineCount; i++) {
            Text::draw_text(canvas, x, y + this->height - this->font->charHeight * (i + 1), this->lines[i].c_str(), font, textColor);
        }
        return true;
    }
};

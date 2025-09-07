#pragma once

#include "canvas.h"
#include "color.h"
#include "font.h"
#include "text.h"

#include <string>
#include <vector>

enum TextAlign {
    RIGHT_TOP,
    RIGHT_MIDDLE,
    RIGHT_BOTTOM,
    CENTER_TOP,
    CENTER_MIDDLE,
    CENTER_BOTTOM,
    LEFT_TOP,
    LEFT_MIDDLE,
    LEFT_BOTTOM
};

struct TextField
{
    int width, height;

    TextField(int width, int height, std::string text, TextAlign textAlign, TextColor colors, Font* font)
    {
        this->width = width;
        this->height = height;
        this->text = text;
        this->textAlign = textAlign;
        this->colors = colors;
        this->font = font;
    }

    void draw(Canvas* canvas, int x, int y)
    {
        std::vector<std::string> lines = wordWrap(this->width / this->font->charWidth);

        int heightDif = this->height - lines.size() * this->font->charHeight;
        for (int i = 0; i < lines.size(); i++) {
            int xPos, yPos;
            switch (this->textAlign) {
                case RIGHT_TOP:
                    xPos = this->width - lines[i].size() * font->charWidth;
                    yPos = this->height - (i + 1) * font->charHeight;
                    break;
                case CENTER_TOP:
                    xPos = (this->width - lines[i].size() * font->charWidth) >> 1;
                    yPos = this->height - (i + 1) * font->charHeight;
                    break;
                case LEFT_TOP:
                    xPos = 0;
                    yPos = this->height - (i + 1) * font->charHeight;
                    break;
                case RIGHT_MIDDLE:
                    xPos = this->width - lines[i].size() * font->charWidth;
                    yPos = this->height - (i + 1) * font->charHeight - (heightDif >> 1);
                    break;
                case CENTER_MIDDLE:
                    xPos = (this->width - lines[i].size() * font->charWidth) >> 1;
                    yPos = this->height - (i + 1) * font->charHeight - (heightDif >> 1);
                    break;
                case LEFT_MIDDLE:
                    xPos = 0;
                    yPos = this->height - (i + 1) * font->charHeight - (heightDif >> 1);
                    break;
                case RIGHT_BOTTOM:
                    xPos = this->width - lines[i].size() * font->charWidth;
                    yPos = this->height - (i + 1) * font->charHeight - heightDif;
                    break;
                case CENTER_BOTTOM:
                    xPos = (this->width - lines[i].size() * font->charWidth) >> 1;
                    yPos = this->height - (i + 1) * font->charHeight - heightDif;
                    break;
                default: // LEFT_BOTTOM
                    xPos = 0;
                    yPos = this->height - (i + 1) * font->charHeight - heightDif;
            }

            Text::draw_line(canvas, x + xPos, y + yPos, lines[i].c_str(), this->colors, this->font);
        }
    }

private:
    std::string text;
    TextAlign textAlign;
    TextColor colors;
    Font* font;

    std::vector<std::string> wordWrap(int maxChar)
    {
        std::vector<std::string> words;
        size_t start = 0, end;
        while ((end = text.find(' ', start)) != std::string::npos) {
            words.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        words.push_back(text.substr(start));

        std::vector<std::string> lines;
        std::string currentLine;
        for (int i = 0; i < words.size(); i++) {
            if (words[i].size() > maxChar) {
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
                lines.push_back(words[i]); // it must be break on charborder
                continue;
            }

            if (currentLine.empty()) {
                currentLine = words[i];
            } else if (currentLine.size() + 1 + words[i].size() <= maxChar) {
                currentLine += " " + words[i];
            } else {
                lines.push_back(currentLine);
                currentLine = words[i];
            }
        }
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }

        return lines;
    }
};

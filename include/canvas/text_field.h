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
    std::string text;
    TextAlign textAlign;
    TextColor colors;
    Font* font;
    int charSpacing;
    int lineSpacing;

    TextField(int width, int height, TextAlign textAlign, std::string text, TextColor colors, Font* font, int charSpacing = 1, int lineSpacing = 0)
    {
        this->width = width;
        this->height = height;
        this->text = text;
        this->textAlign = textAlign;
        this->colors = colors;
        this->font = font;
        this->charSpacing = charSpacing;
        this->lineSpacing = lineSpacing;
    }

    void draw(Canvas* canvas, int x, int y)
    {
        Rectangle::draw(canvas, x, y, this->width, this->height, EGA_RED, 0x55555555);

        std::vector<std::string> lines = wordWrap();

        int heightDiff = this->height - lines.size() * (this->font->charHeight + this->lineSpacing);
        for (int i = 0; i < lines.size(); i++) {
            int xPos, yPos;
            // switch (this->textAlign) {
            //     case RIGHT_TOP:
            //         xPos = this->width - lines[i].size() * font->spaceWidth;
            //         yPos = this->height - (i + 1) * font->charHeight;
            //         break;
            //     case CENTER_TOP:
            //         xPos = (this->width - lines[i].size() * font->spaceWidth) >> 1;
            //         yPos = this->height - (i + 1) * font->charHeight;
            //         break;
            //     case LEFT_TOP:
            //         xPos = 0;
            //         yPos = this->height - (i + 1) * font->charHeight;
            //         break;
            //     case RIGHT_MIDDLE:
            //         xPos = this->width - lines[i].size() * font->spaceWidth;
            //         yPos = this->height - (i + 1) * font->charHeight - (heightDiff >> 1);
            //         break;
            //     case CENTER_MIDDLE:
            //         xPos = (this->width - lines[i].size() * font->spaceWidth) >> 1;
            //         yPos = this->height - (i + 1) * font->charHeight - (heightDiff >> 1);
            //         break;
            //     case LEFT_MIDDLE:
            //         xPos = 0;
            //         yPos = this->height - (i + 1) * font->charHeight - (heightDiff >> 1);
            //         break;
            //     case RIGHT_BOTTOM:
            //         xPos = this->width - lines[i].size() * font->spaceWidth;
            //         yPos = this->height - (i + 1) * font->charHeight - heightDiff;
            //         break;
            //     case CENTER_BOTTOM:
            //         xPos = (this->width - lines[i].size() * font->spaceWidth) >> 1;
            //         yPos = this->height - (i + 1) * font->charHeight - heightDiff;
            //         break;
            //     default: // LEFT_BOTTOM
                    xPos = 0;
                    yPos = this->height - (i + 1) * font->charHeight - heightDiff;
            // }
            Text::draw_line(canvas, x + xPos, y + yPos, lines[i].c_str(), this->colors, this->font, this->charSpacing, this->lineSpacing);
        }
    }

private:
    std::vector<std::string> wordWrap()
    {
        std::vector<std::string> result;

        std::vector<std::string> lines = explode('\n', this->text);
        for (int j = 0; j < lines.size(); j++) {
            std::vector<std::string> words = explode(' ', lines[j]);
            std::string currentLine;
            int currentLineWidth = 0;
            for (int i = 0; i < words.size(); i++) {
                int wordWidth = getWordWidth(words[i]);

                if (wordWidth >= this->width) {
                    if (!currentLine.empty()) {
                        result.push_back(currentLine);
                        currentLine.clear();
                    }
                }

                if (currentLine.empty()) {
                    currentLine = words[i];
                    currentLineWidth = wordWidth;
                } else if (currentLineWidth + this->font->charset[0]->width + wordWidth <= this->width) {
                    currentLine += " " + words[i];
                    currentLineWidth += this->font->charset[0]->width + wordWidth;
                } else {
                    result.push_back(currentLine);
                    currentLine = words[i];
                }
            }
            result.push_back(currentLine);
        }
        return result;
    }

    std::vector<std::string> explode(char separator, std::string text)
    {
        std::vector<std::string> result;
        size_t start = 0, end;
        while ((end = text.find(separator, start)) != std::string::npos) {
            result.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        result.push_back(text.substr(start));
        return result;
    }

    int getWordWidth(std::string text)
    {
        int result = 0;
        for (int i = 0; i < text.size(); i++) {
            result += this->font->charset[
                (text[i] - 32 < 1 || text[i] - 32 > font->charCount) ? 0 : text[i] - 32
            ]->width + this->charSpacing;
        }
        return result;
    }
};

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
    int charSpacing = 0;
    int lineSpacing = 0;

    TextField(int width, int height, TextAlign textAlign, std::string text, TextColor colors, Font* font, int charSpacing = 1, int lineSpacing = 0)
    {
        this->width = width;
        this->height = height;
        this->text = text;
        this->textAlign = textAlign;
        this->colors = colors;
        this->font = font;
        if (this->font->type == MONOSPACED) this->charSpacing--;
        this->lineSpacing = lineSpacing;
    }

    void draw(Canvas* canvas, int x, int y)
    {
        std::vector<std::pair<std::string, int>> lines = wordWrap();

        int heightDiff = this->height - lines.size() * (this->font->charHeight + this->lineSpacing);
        for (int i = 0; i < lines.size(); i++) {
            int xPos, yPos;
            switch (this->textAlign) {
                case RIGHT_TOP:
                    xPos = this->width - lines[i].second;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing);
                    break;
                case CENTER_TOP:
                    xPos = (this->width - lines[i].second) >> 1;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing);
                    break;
                case LEFT_TOP:
                    xPos = 0;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing);
                    break;
                case RIGHT_MIDDLE:
                    xPos = this->width - lines[i].second;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - (heightDiff >> 1);
                    break;
                case CENTER_MIDDLE:
                    xPos = (this->width - lines[i].second) >> 1;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - (heightDiff >> 1);
                    break;
                case LEFT_MIDDLE:
                    xPos = 0;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - (heightDiff >> 1);
                    break;
                case RIGHT_BOTTOM:
                    xPos = this->width - lines[i].second;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - heightDiff;
                    break;
                case CENTER_BOTTOM:
                    xPos = (this->width - lines[i].second) >> 1;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - heightDiff;
                    break;
                default: // LEFT_BOTTOM
                    xPos = 0;
                    yPos = this->height - (i + 1) * (this->font->charHeight + this->lineSpacing) - heightDiff;
            }
            Text::draw_line(canvas, x + xPos, y + yPos, lines[i].first.c_str(), this->colors, this->font, this->charSpacing, this->lineSpacing);
        }
    }

private:
    std::vector<std::pair<std::string, int>> wordWrap()
    {
        std::vector<std::pair<std::string, int>> result;

        std::vector<std::string> lines = explodeByChar(this->text, '\n');
        for (int i = 0; i < lines.size(); i++) {
            std::vector<std::string> words = explodeByChar(lines[i], ' ');
            std::string currentLine;
            int currentLineWidth = 0;
            for (int j = 0; j < words.size(); j++) {
                int wordWidth = getWordWidth(words[j]);

                if (wordWidth >= this->width) {
                    if (!currentLine.empty()) {
                        result.push_back(std::make_pair(currentLine, currentLineWidth));
                        currentLine.clear();
                    }
                    std::vector<std::pair<std::string, int>> splittedWords = explodeByWidth(words[j], this->width);
                    for (int k = 0; k < splittedWords.size(); k++) {
                        result.push_back(std::make_pair(splittedWords[k].first, splittedWords[k].second));
                    }
                    continue;
                }

                if (currentLine.empty()) {
                    currentLine = words[j];
                    currentLineWidth = wordWidth;
                } else if (currentLineWidth + this->font->charset[0]->width + wordWidth < this->width) {
                    currentLine += " " + words[j];
                    currentLineWidth += this->font->charset[0]->width + this->charSpacing + wordWidth;
                } else {
                    result.push_back(std::make_pair(currentLine, currentLineWidth));
                    currentLine = words[j];
                    currentLineWidth = wordWidth;
                }
            }
            result.push_back(std::make_pair(currentLine, currentLineWidth));
        }
        return result;
    }

    std::vector<std::string> explodeByChar(std::string text, char separator)
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

    std::vector<std::pair<std::string, int>> explodeByWidth(std::string text, int maxWidth)
    {
        std::vector<std::pair<std::string, int>> result;

        int wordWidth = 0;
        size_t start = 0, end;
        for (int i = 0; i < text.size(); i++) {

            int charWidth = this->font->charset[(text[i] - 32 < 1 || text[i] - 32 >= font->charCount) ? 0 : text[i] - 32]->width + this->charSpacing;

            if (wordWidth + charWidth >= maxWidth) {
                result.push_back(std::make_pair(text.substr(start, i - start), wordWidth));
                wordWidth = charWidth;
                start = i;
            } else wordWidth += charWidth;
        }
        result.push_back(std::make_pair(text.substr(start), wordWidth));

        return result;
    }

    int getWordWidth(std::string text)
    {
        int result = 0;
        for (int i = 0; i < text.size(); i++) {
            result += this->font->charset[(text[i] - 32 < 1 || text[i] - 32 >= font->charCount) ? 0 : text[i] - 32]->width + this->charSpacing;
        }
        return result;
    }
};

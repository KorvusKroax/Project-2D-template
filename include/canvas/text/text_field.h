#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"
#include "text/text.h"

#include <string>
#include <vector>

struct TextField
{
    int width, height;
    Font *font;

    TextField(int width, int height, Font *font, std::string text):
        width(width), height(height), font(font)
    {
        std::vector<std::vector<int>> rows = explodeByCodepoint(this->font->utf8ToCodepoints(text), '\n');
        for (std::vector<int> row : rows) {
            std::vector<std::vector<int>> words = explodeByCodepoint(row, ' ');

            std::vector<int> currentLine;
            float currentLineWidth = 0;

            for (std::vector<int> word : words) {
                float wordWidth = getTextWidth(word);

                if (wordWidth == 0) continue;

                if (wordWidth > this->width) {
                    if (currentLineWidth > 0) {
                        lines.push_back(std::make_pair(currentLine, currentLineWidth));
                        currentLine.clear();
                        currentLineWidth = 0;
                    }

                    std::vector<std::pair<std::vector<int>, float>> splittedLongWordParts = explodeByFieldWidth(word);
                    for (int i = 0; i < splittedLongWordParts.size() - 1; i++) {
                        lines.push_back(std::make_pair(splittedLongWordParts[i].first, splittedLongWordParts[i].second));
                    }
                    currentLine = splittedLongWordParts.back().first;
                    currentLineWidth = splittedLongWordParts.back().second;
                    continue;
                }

                if (currentLineWidth == 0) {
                    currentLine = word;
                    currentLineWidth = wordWidth;
                    continue;
                }

                float spaceWidth = (
                    stbtt_GetCodepointKernAdvance(&this->font->info, currentLine.back(), ' ') +
                    this->font->getGlyph(' ')->advanceWidth +
                    stbtt_GetCodepointKernAdvance(&this->font->info, ' ', word.front())
                ) * this->font->scale;

                if (currentLineWidth + spaceWidth + wordWidth <= this->width) {
                    currentLine.push_back(' ');
                    currentLine.insert(currentLine.end(), word.begin(), word.end());
                    currentLineWidth += spaceWidth + wordWidth;
                    continue;
                }

                lines.push_back(std::make_pair(currentLine, currentLineWidth));
                currentLine = word;
                currentLineWidth = wordWidth;
            }

            lines.push_back(std::make_pair(currentLine, currentLineWidth));
        }
    }

    void draw(Canvas *canvas, int x, float y, Color color, float lineSpacingMultiplier = 1.0f)
    {
        float lineHeight = (this->font->ascent - this->font->descent + this->font->lineGap) * this->font->scale * lineSpacingMultiplier;

        y += (this->lines.size() - 1) * lineHeight;

        for (std::pair<std::vector<int>, float> line : this->lines) {
            Text::draw_line(canvas, x, y, line.first, this->font, color, CLEAR);
            y -= lineHeight;
        }
    }

private:
    std::vector<std::pair<std::vector<int>, float>> lines;

    std::vector<std::vector<int>> explodeByCodepoint(const std::vector<int> &text, int separator)
    {
        std::vector<std::vector<int>> result;

        std::vector<int> textPart;
        for (int codepoint : text) {
            if (codepoint == separator) {
                result.push_back(textPart);
                textPart.clear();
            } else {
                textPart.push_back(codepoint);
            }
        }
        result.push_back(textPart);

        return result;
    }

    float getTextWidth(const std::vector<int> &text)
    {
        float textWidth = 0;

        for (int i = 0; i < text.size(); i++) {
            textWidth +=
                (i ? stbtt_GetCodepointKernAdvance(&this->font->info, text[i - 1], text[i]) : 0) +
                this->font->getGlyph(text[i])->advanceWidth;
        }

        return textWidth * this->font->scale;
    }

    std::vector<std::pair<std::vector<int>, float>> explodeByFieldWidth(const std::vector<int> &text)
    {
        std::vector<std::pair<std::vector<int>, float>> result;

        std::vector<int> line;
        float lineWidth = 0;
        for (int i = 0; i < text.size(); i++) {

            int kern = i ? stbtt_GetCodepointKernAdvance(&this->font->info, text[i - 1], text[i]) * this->font->scale : 0;
            float charWidth = this->font->getGlyph(text[i])->advanceWidth * this->font->scale;

            if (lineWidth + kern + charWidth > this->width) {
                result.push_back(std::make_pair(line, lineWidth));
                line.assign(1, text[i]);
                lineWidth = charWidth;
                continue;
            }

            line.push_back(text[i]);
            lineWidth += kern + charWidth;
        }
        result.push_back(std::make_pair(line, lineWidth));

        return result;
    }
};

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

    TextField(int width, int height, Font *font, std::string text, const unsigned int tabSize = 4):
        width(width), height(height), font(font)
    {
        update(text, tabSize);
    }

    void update(std::string text, const unsigned int tabSize = 4)
    {
        lines.clear();

        // replace tabs to spaces
        std::string spaces(tabSize, ' ');
        int pos = 0;
        while ((pos = text.find('\t', pos)) != std::string::npos) {
            text.replace(pos, 1, spaces);
            pos += tabSize;
        }

        std::vector<std::pair<int, std::vector<int>>> rows = explodeByCodepoint(this->font->utf8ToCodepoints(text), '\n');
        for (std::pair<int, std::vector<int>> row : rows) {

            std::vector<int> currentLine;
            float currentLineWidth = 0;

            // process multiple '\n'
            for (int i = 0; i < row.first - 1; i++) {
                lines.push_back(std::make_pair(currentLine, currentLineWidth));
            }

            std::vector<std::pair<int, std::vector<int>>> words = explodeByCodepoint(row.second, ' ');
            for (std::pair<int, std::vector<int>> word : words) {
                float wordWidth = getTextWidth(word.second);

                // break long words
                if (wordWidth > this->width) {
                    if (currentLineWidth > 0) {
                        lines.push_back(std::make_pair(currentLine, currentLineWidth));
                        currentLine.clear();
                        currentLineWidth = 0;
                    }
                    std::vector<std::pair<std::vector<int>, float>> splittedLongWordParts = explodeByFieldWidth(word.second);
                    for (int i = 0; i < splittedLongWordParts.size() - 1; i++) {
                        lines.push_back(std::make_pair(splittedLongWordParts[i].first, splittedLongWordParts[i].second));
                    }
                    currentLine = splittedLongWordParts.back().first;
                    currentLineWidth = splittedLongWordParts.back().second;
                    continue;
                }

                // calculate spaces width (even multiple spaces)
                float spaceWidth = currentLineWidth ? stbtt_GetCodepointKernAdvance(&this->font->info, currentLine.back(), ' ') : 0;
                for (int i = 0; i < word.first - 1; i++) {
                    spaceWidth +=
                        this->font->getGlyph(' ')->advanceWidth +
                        stbtt_GetCodepointKernAdvance(&this->font->info, ' ', ' ');
                }
                spaceWidth +=
                    this->font->getGlyph(' ')->advanceWidth +
                    stbtt_GetCodepointKernAdvance(&this->font->info, ' ', word.second.front());
                spaceWidth *= this->font->scale;

                if (currentLineWidth + spaceWidth + wordWidth <= this->width) {
                    // add spaces and the current word to the line
                    currentLine.insert(currentLine.end(), word.first, ' ');
                    currentLine.insert(currentLine.end(), word.second.begin(), word.second.end());
                    currentLineWidth += spaceWidth + wordWidth;
                    continue;
                }

                // break the line and start the next line with the current word
                lines.push_back(std::make_pair(currentLine, currentLineWidth));
                currentLine = word.second;
                currentLineWidth = wordWidth;
            }

            lines.push_back(std::make_pair(currentLine, currentLineWidth));
        }
    }

    void draw(Canvas *canvas, int x, float y, Color color, Color shadow = CLEAR, float lineSpacingMultiplier = 1.0f)
    {
        float lineHeight = (this->font->ascent - this->font->descent + this->font->lineGap) * this->font->scale * lineSpacingMultiplier;

        y += (this->lines.size() - 1) * lineHeight;

        for (std::pair<std::vector<int>, float> line : this->lines) {
            Text::draw_line(canvas, x, y, line.first, this->font, color, shadow);
            y -= lineHeight;
        }
    }

private:
    std::vector<std::pair<std::vector<int>, float>> lines;

    std::vector<std::pair<int, std::vector<int>>> explodeByCodepoint(const std::vector<int> &text, int separator)
    {
        std::vector<std::pair<int, std::vector<int>>> result; // pair.first = separator count before string, pair.second = string

        std::vector<int> textPart;
        int separatorCount = 0;
        for (int codepoint : text) {
            if (codepoint != separator) {
                textPart.push_back(codepoint);
                continue;
            }
            if (!textPart.empty()) {
                result.push_back(std::make_pair(separatorCount, textPart));
                textPart.clear();
                separatorCount = 1;
            } else separatorCount++;
        }
        result.push_back(std::make_pair(separatorCount, textPart));

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

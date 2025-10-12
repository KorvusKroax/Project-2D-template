#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"
#include "text/text.h"

#include "rectangle.h"

#include <string>
#include <vector>

struct TextField
{
    int width, height;
    std::string text;
    Font *font;
    Color color;
    float lineSpacingMultiplier;

    TextField(int width, int height, std::string text, Font *font, Color color, float lineSpacingMultiplier = 1.0f):
        width(width), height(height), text(text), font(font), color(color), lineSpacingMultiplier(lineSpacingMultiplier)
    {
        std::vector<std::string> rows = explodeByChar(this->text, '\n');
        for (std::string row : rows) {
            std::string currentLine;
            float currentLineWidth = 0;
            std::vector<std::string> words = explodeByChar(row, ' ');
            for (std::string word : words) {
                float wordWidth = getTextWidth(word);

                // check if the word longest than the field
                if (wordWidth > this->width) {
                    if (!currentLine.empty()) {
                        // finish the line if not empty and clear the next
                        lines.push_back(std::make_pair(currentLine, currentLineWidth));
                        currentLine.clear();
                    }

                    // split long word more lines
                    std::vector<std::pair<std::string, float>> splittedWords = explodeByWidth(word);
                    for (int i = 0; i < splittedWords.size() - 1; i++) {
                        lines.push_back(std::make_pair(splittedWords[i].first, splittedWords[i].second));
                    }
                    // long word's end added to current line
                    currentLine = splittedWords[splittedWords.size() - 1].first;
                    currentLineWidth = splittedWords[splittedWords.size() - 1].second;
                    continue;
                }

                if (currentLine.empty()) {
                    // first word in the line
                    currentLine = word;
                    currentLineWidth = wordWidth;
                    continue;
                }

                int spaceWidth = getSpaceWidthWithKerning((unsigned char)currentLine.back(), (unsigned char)word.front());
                if (currentLineWidth + spaceWidth + wordWidth <= this->width) {
                    // adding next word to the line
                    currentLine += " " + word;
                    currentLineWidth += spaceWidth + wordWidth;
                    continue;
                }

                // finish line and start next line
                lines.push_back(std::make_pair(currentLine, currentLineWidth));
                currentLine = word;
                currentLineWidth = wordWidth;
            }

            // finish the last line
            lines.push_back(std::make_pair(currentLine, currentLineWidth));
        }
    }

    void draw(Canvas *canvas, int x, float y)
    {
        Rectangle::draw(canvas, x, y, this->width, this->height, C64_VICE_RED);

        float lineHeight = (this->font->ascent - this->font->descent + this->font->lineGap) * this->font->scale * this->lineSpacingMultiplier;

        y += (this->lines.size() - 1) * lineHeight;

        for (std::pair<std::string, float> line : this->lines) {
            Text::draw_line(canvas, x, y, line.first, this->font, this->color, CLEAR);
            y -= lineHeight;
        }
    }

private:
    std::vector<std::pair<std::string, float>> lines;

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

    float getTextWidth(std::string text)
    {
        std::vector<int> codepoints = this->font->utf8ToCodepoints(text);

        Glyph *nextGlyph, *currGlyph = this->font->getGlyph(codepoints[0]);
        float textWidth = currGlyph->advanceWidth * this->font->scale;

        for (int i = 1; i < codepoints.size(); i++) {
            nextGlyph = this->font->getGlyph(codepoints[i]);
            textWidth +=
                stbtt_GetGlyphKernAdvance(&this->font->info, currGlyph->index, nextGlyph->index) * this->font->scale +
                nextGlyph->advanceWidth * this->font->scale;
            currGlyph = nextGlyph;
        }

        return textWidth;
    }

    float getSpaceWidthWithKerning(unsigned char lastChar, unsigned char firstChar)
    {
        Glyph *lastGlyph = this->font->getGlyph(lastChar); // this->font->utf8ToCodepoint(lastChar)
        Glyph *spaceGlyph = this->font->getGlyph(' '); // this->font->utf8ToCodepoint(" ")
        Glyph *firstGlyph = this->font->getGlyph(firstChar); // this->font->utf8ToCodepoint(firstChar)

        float kerningLeft  = stbtt_GetGlyphKernAdvance(&this->font->info, lastGlyph->index, spaceGlyph->index) * this->font->scale;
        int spaceWidth = spaceGlyph->advanceWidth * this->font->scale;
        float kerningRight = stbtt_GetGlyphKernAdvance(&this->font->info, spaceGlyph->index, firstGlyph->index) * this->font->scale;

        return kerningLeft + spaceWidth + kerningRight;
    }

    std::vector<std::pair<std::string, float>> explodeByWidth(std::string text)
    {
        std::vector<std::pair<std::string, float>> result;

        std::vector<int> codepoints = this->font->utf8ToCodepoints(text);

        Glyph *nextGlyph, *currGlyph = this->font->getGlyph(codepoints[0]);
        std::string line(1, text[0]);
        float lineWidth = currGlyph->advanceWidth * this->font->scale;

        for (int i = 1; i < text.size(); i++) {
            nextGlyph = this->font->getGlyph(codepoints[i]);
            float charWidth =
                stbtt_GetGlyphKernAdvance(&this->font->info, currGlyph->index, nextGlyph->index) * this->font->scale +
                nextGlyph->advanceWidth * this->font->scale;

            if (lineWidth + charWidth > this->width) {
                result.push_back(std::make_pair(line, lineWidth));
                line = text[i];
                lineWidth = charWidth;
                continue;
            }

            line += text[i];
            lineWidth += charWidth;
            currGlyph = nextGlyph;
        }
        result.push_back(std::make_pair(line, lineWidth));

        return result;
    }
};

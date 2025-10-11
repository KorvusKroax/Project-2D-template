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
    Font* font;
    Color color;
    float lineSpacingMultiplier;

    TextField(int width, int height, std::string text, Font* font, Color color, float lineSpacingMultiplier = 1.0f):
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
                    std::vector<std::pair<std::string, int>> splittedWords = explodeByWidth(word);
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

    void draw(Canvas* canvas, int x, float y)
    {
        Rectangle::draw(canvas, x, y, this->width, this->height, C64_VICE_RED);

        float lineHeight = (this->font->ascent - this->font->descent + this->font->lineGap) * this->font->scale * this->lineSpacingMultiplier;

        y += (this->lines.size() - 1) * lineHeight;

        for (std::pair<std::string, int> line : this->lines) {
            Text::draw_line(canvas, x, y, line.first, this->font, this->color, CLEAR);
            y -= lineHeight;
        }
    }

private:
    std::vector<std::pair<std::string, int>> lines;

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
        float textWidth = 0;

        std::vector<int> codepoints = this->font->utf8ToCodepoints(text);
        for (int i = 0; i < codepoints.size(); i++) {
            Glyph* glyph = this->font->getGlyph(codepoints[i]);
            textWidth += glyph->advanceWidth * this->font->scale;


// ez itt nem jó: glyph indexet kell megadni neki nem a char-t
            if (i + 1 < codepoints.size()) {
                textWidth += stbtt_GetGlyphKernAdvance(&this->font->info, codepoints[i], codepoints[i + 1]) * this->font->scale;
            }
        }

        return textWidth;
    }

    float getSpaceWidthWithKerning(unsigned char lastChar, unsigned char firstChar)
    {
        int lastGlyphFirst   = stbtt_FindGlyphIndex(&this->font->info, lastChar);
        int spaceGlyph       = stbtt_FindGlyphIndex(&this->font->info, ' ');
        int firstGlyphSecond = stbtt_FindGlyphIndex(&this->font->info, firstChar);

        float kerningLeft  = stbtt_GetGlyphKernAdvance(&this->font->info, lastGlyphFirst, spaceGlyph) * this->font->scale;
        float kerningRight = stbtt_GetGlyphKernAdvance(&this->font->info, spaceGlyph, firstGlyphSecond) * this->font->scale;

        Glyph* glyph = this->font->getGlyph(' '); // this->font->utf8ToCodepoint(" ")
        int spaceWidth = glyph->advanceWidth * this->font->scale;

        return kerningLeft + spaceWidth + kerningRight;
    }

    std::vector<std::pair<std::string, int>> explodeByWidth(std::string text)
    {
        std::vector<std::pair<std::string, int>> result;

        // std::vector<int> codepoints = this->font->utf8ToCodepoints(text);

        std::string line;
        float lineWidth = 0;
        for (int i = 0; i < text.size(); i++) {


            Glyph* glyph = this->font->getGlyph(this->font->utf8ToCodepoint(&text[i]));
            int charWidth = glyph->advanceWidth * this->font->scale;

            if (lineWidth + charWidth > this->width) {
                result.push_back(std::make_pair(line, lineWidth));
                line = text[i];
                lineWidth = charWidth;
            } else {
                line += text[i];
                lineWidth += charWidth;


// ez itt nem jó: glyph indexet kell megadni neki nem a char-t
                if (i + 1 < text.size()) {
                    lineWidth += stbtt_GetGlyphKernAdvance(&this->font->info, text[i], text[i + 1]) * this->font->scale;
                }
            }

        }
        result.push_back(std::make_pair(line, lineWidth));

        return result;
    }


    // void wordWrap()
    // {
    //     std::vector<std::vector<int>> lines = explodeByChar(this->font->utf8_to_codepoints(this->text), '\n');
    //     for (int i = 0; i < lines.size(); i++) {

    //         std::vector<std::vector<int>> words = explodeByChar(lines[i], ' ');
    //         std::string currentLine;
    //         int currentLineWidth = 0;
    //         for (int j = 0; j < words.size(); j++) {
    //             float wordWidth = getWordWidth(words[j]);

    //             if (wordWidth >= this->width) {
    //                 if (!currentLine.empty()) {
    //                     rows.push_back(std::make_pair(currentLine, currentLineWidth));
    //                     currentLine.clear();
    //                 }
    //                 std::vector<std::pair<std::string, int>> splittedWords = explodeByWidth(words[j], this->width);
    //                 for (int k = 0; k < splittedWords.size(); k++) {
    //                     rows.push_back(std::make_pair(splittedWords[k].first, splittedWords[k].second));
    //                 }
    //                 continue;
    //             }

    //             if (currentLine.empty()) {
    //                 currentLine = words[j];
    //                 currentLineWidth = wordWidth;
    //             } else if (currentLineWidth + this->font->charset[0]->width + wordWidth < this->width) {
    //                 currentLine += " " + words[j];
    //                 currentLineWidth += this->font->charset[0]->width + this->charSpacing + wordWidth;
    //             } else {
    //                 result.push_back(std::make_pair(currentLine, currentLineWidth));
    //                 currentLine = words[j];
    //                 currentLineWidth = wordWidth;
    //             }
    //         }
    //         result.push_back(std::make_pair(currentLine, currentLineWidth));
    //     }
    // }

    // std::vector<std::pair<std::string, int>> explodeByWidth(std::vector<int> codepoints, int maxWidth)
    // {
    //     std::vector<std::pair<std::string, int>> result;

    //     int wordWidth = 0;
    //     size_t start = 0, end;
    //     for (int i = 0; i < text.size(); i++) {

    //         int charWidth = this->font->charset[(text[i] - 32 < 1 || text[i] - 32 >= font->charCount) ? 0 : text[i] - 32]->width + this->charSpacing;

    //         if (wordWidth + charWidth >= maxWidth) {
    //             result.push_back(std::make_pair(text.substr(start, i - start), wordWidth));
    //             wordWidth = charWidth;
    //             start = i;
    //         } else wordWidth += charWidth;
    //     }
    //     result.push_back(std::make_pair(text.substr(start), wordWidth));

    //     return result;
    // }
};

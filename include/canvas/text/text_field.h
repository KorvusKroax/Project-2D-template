#pragma once

#include "canvas.h"
#include "color/color.h"
#include "font.h"
#include "text.h"

#include <string>
#include <vector>

enum TextAlign
{
    LEFT_TOP,
    LEFT_CENTER,
    LEFT_BOTTOM,
    CENTER_TOP,
    CENTER_CENTER,
    CENTER_BOTTOM,
    RIGHT_TOP,
    RIGHT_CENTER,
    RIGHT_BOTTOM
};

struct TextField
{
    struct Options {
        int width = 100;
        int height = 100;
        TextAlign textAlign = LEFT_BOTTOM;
        std::string text = "";
        Text::Options textOptions;

        bool operator != (const Options& other) const {
            return
                this->width != other.width ||
                this->text != other.text ||
                this->textOptions.font != other.textOptions.font ||
                this->textOptions.tabSize != other.textOptions.tabSize ||
                this->textOptions.lineHeightScale != other.textOptions.lineHeightScale;
        }
    };

    Options opts;
    std::vector<std::pair<std::vector<int>, float>> lines;

    explicit TextField(const Options &initialOpts)
    {
        this->opts = initialOpts;
        buildLines();
    }

    bool draw(Canvas* canvas, int x, int y, Options* newOpts = nullptr)
    {
        if (newOpts) {
            if (this->opts != *newOpts)
                if (!buildLines()) return false;
            this->opts = *newOpts;
        }

        float baseLineHeight = (this->opts.textOptions.font->ascent - this->opts.textOptions.font->descent + this->opts.textOptions.font->lineGap) * this->opts.textOptions.font->scale;
        float lineHeight = baseLineHeight * this->opts.textOptions.lineHeightScale;
        float lineSpacing = lineHeight - baseLineHeight;

        float xPos, yPos;

        switch (this->opts.textAlign) {
            case RIGHT_TOP:
            case CENTER_TOP:
            case LEFT_TOP:
                yPos = y + this->opts.height - lineHeight + lineSpacing;
                break;
            case RIGHT_CENTER:
            case CENTER_CENTER:
            case LEFT_CENTER:
                yPos = y + (this->opts.height + ((this->lines.size() - 2) * lineHeight) + lineSpacing) * .5f;
                break;
            default: // BOTTOM
                yPos = y + (this->lines.size() - 1) * lineHeight;
                break;
        }

        for (std::pair<std::vector<int>, float> line : this->lines) {
            switch (this->opts.textAlign) {
                case RIGHT_TOP:
                case RIGHT_CENTER:
                case RIGHT_BOTTOM:
                    xPos = x + (this->opts.width - line.second);
                    break;
                case CENTER_TOP:
                case CENTER_CENTER:
                case CENTER_BOTTOM:
                    xPos = x + (this->opts.width - line.second) * .5f;
                    break;
                default: // LEFT
                    xPos = x;
                    break;
            }

            Text::draw_line(canvas, xPos, yPos, line.first, this->opts.textOptions);
            yPos -= lineHeight;
        }

        return true;
    }

private:
    bool buildLines()
    {
        if (!this->opts.textOptions.font) {
            printf("ERROR: TextField::buildLines() -- Missing font!\n");
            return false;
        }

        lines.clear();

        // replace tabs to spaces
        std::string spaces(this->opts.textOptions.tabSize, ' ');
        int pos = 0;
        while ((pos = this->opts.text.find('\t', pos)) != std::string::npos) {
            this->opts.text.replace(pos, 1, spaces);
            pos += this->opts.textOptions.tabSize;
        }

        std::vector<std::pair<int, std::vector<int>>> rows = explodeByCodepoint(this->opts.textOptions.font->utf8ToCodepoints(this->opts.text), '\n');
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
                if (wordWidth > this->opts.width) {
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
                float spaceWidth = 0;
                if (word.first > 0) {
                    spaceWidth = currentLineWidth ? stbtt_GetCodepointKernAdvance(&this->opts.textOptions.font->info, currentLine.back(), ' ') : 0;
                    for (int i = 0; i < word.first - 1; i++) { // word.first: separator count (means spaces)
                        spaceWidth +=
                            this->opts.textOptions.font->getGlyph(' ')->advanceWidth +
                            stbtt_GetCodepointKernAdvance(&this->opts.textOptions.font->info, ' ', ' ');
                    }
                    spaceWidth +=
                        this->opts.textOptions.font->getGlyph(' ')->advanceWidth +
                        stbtt_GetCodepointKernAdvance(&this->opts.textOptions.font->info, ' ', word.second.front());
                    spaceWidth *= this->opts.textOptions.font->scale;
                }

                if (currentLineWidth + spaceWidth + wordWidth <= this->opts.width) {
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

        return true;
    }

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
                (i ? stbtt_GetCodepointKernAdvance(&this->opts.textOptions.font->info, text[i - 1], text[i]) : 0) +
                this->opts.textOptions.font->getGlyph(text[i])->advanceWidth;
        }

        return textWidth * this->opts.textOptions.font->scale;
    }

    std::vector<std::pair<std::vector<int>, float>> explodeByFieldWidth(const std::vector<int> &text)
    {
        std::vector<std::pair<std::vector<int>, float>> result;

        std::vector<int> line;
        float lineWidth = 0;
        for (int i = 0; i < text.size(); i++) {

            int kern = i ? stbtt_GetCodepointKernAdvance(&this->opts.textOptions.font->info, text[i - 1], text[i]) * this->opts.textOptions.font->scale : 0;
            float charWidth = this->opts.textOptions.font->getGlyph(text[i])->advanceWidth * this->opts.textOptions.font->scale;

            if (lineWidth + kern + charWidth > this->opts.width) {
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

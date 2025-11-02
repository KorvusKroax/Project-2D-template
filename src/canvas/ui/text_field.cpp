#include "ui/text_field.h"

void TextField::draw(Canvas* canvas)
{
    buildLines();
    drawLines(canvas);
}

void TextField::drawLines(Canvas* canvas)
{
    float lineHeight = Text::calcLineHeight(this->opts);
    float lineSpacing = lineHeight - this->opts.font->calcFontHeight();

    float xPos, yPos;
    switch (this->textAlign) {
        case RIGHT_TOP:
        case CENTER_TOP:
        case LEFT_TOP:
            yPos = this->y + this->height - lineHeight + lineSpacing;
            break;
        case RIGHT_CENTER:
        case CENTER_CENTER:
        case LEFT_CENTER:
            yPos = this->y + (this->height - lineHeight + ((this->lines.size() - 1) * lineHeight) + lineSpacing) * .5f;
            break;
        default: // BOTTOM
            yPos = this->y + (this->lines.size() - 1) * lineHeight;
            break;
    }

    for (std::pair<std::vector<int>, float> line : this->lines) {
        switch (this->textAlign) {
            case RIGHT_TOP:
            case RIGHT_CENTER:
            case RIGHT_BOTTOM:
                xPos = this->x + (this->width - line.second);
                break;
            case CENTER_TOP:
            case CENTER_CENTER:
            case CENTER_BOTTOM:
                xPos = this->x + (this->width - line.second) * .5f;
                break;
            default: // LEFT
                xPos = this->x;
                break;
        }

        Text::draw_line(canvas, xPos, yPos, line.first, this->opts);
        yPos -= lineHeight;
    }
}

void TextField::buildLines()
{
    // replace tabs to spaces
    std::string spaces(this->opts.tabSize, ' ');
    int pos = 0;
    while ((pos = this->text.find('\t', pos)) != std::string::npos) {
        this->text.replace(pos, 1, spaces);
        pos += this->opts.tabSize;
    }

    // set text to codepoints and break text to rows by '\n'
    //      (row.first = separator count before string, row.second = string as codepoints)
    std::vector<std::pair<int, std::vector<int>>> rows =
        explodeByCodepoint(Text::utf8ToCodepoints(this->text), '\n');

    this->lines.clear();

    for (std::pair<int, std::vector<int>> row : rows) {

        // process multiple '\n'
        for (int i = 0; i < row.first - 1; i++) { // row.first: separator count (means line breaks)
            this->lines.emplace_back(std::vector<int>{}, 0.0f);
        }

        std::vector<int> currentLine;
        float currentLineWidth = 0.0f;

        std::vector<std::pair<int, std::vector<int>>> words = explodeByCodepoint(row.second, ' ');
        for (std::pair<int, std::vector<int>> word : words) {
            float wordWidth = Text::calcLineWidth(word.second, this->opts);

            // break long words
            if (wordWidth > this->width) {
                if (currentLineWidth > 0) {
                    this->lines.emplace_back(currentLine, currentLineWidth);
                    currentLine.clear();
                    currentLineWidth = 0;
                }

                std::vector<std::pair<std::vector<int>, float>> splittedLongWordParts =
                    explodeByFieldWidth(word.second);

                for (int i = 0; i < splittedLongWordParts.size() - 1; i++) {
                    this->lines.emplace_back(splittedLongWordParts[i].first, splittedLongWordParts[i].second);
                }
                currentLine = splittedLongWordParts.back().first;
                currentLineWidth = splittedLongWordParts.back().second;
                continue;
            }

            // calculate spaces width (even multiple spaces)
            float spaceWidth = 0;
            if (word.first > 0) {
                spaceWidth = currentLineWidth ? stbtt_GetCodepointKernAdvance(&this->opts.font->info, currentLine.back(), ' ') : 0;
                for (int i = 0; i < word.first - 1; i++) { // word.first: separator count (means spaces)
                    spaceWidth +=
                        this->opts.font->getGlyph(' ')->advanceWidth +
                        stbtt_GetCodepointKernAdvance(&this->opts.font->info, ' ', ' ');
                }
                spaceWidth +=
                    this->opts.font->getGlyph(' ')->advanceWidth +
                    stbtt_GetCodepointKernAdvance(&this->opts.font->info, ' ', word.second.front());
                spaceWidth *= this->opts.font->scale;
            }

            if (currentLineWidth + spaceWidth + wordWidth <= this->width) {
                // add spaces and the current word to the line
                currentLine.insert(currentLine.end(), word.first, ' ');
                currentLine.insert(currentLine.end(), word.second.begin(), word.second.end());
                currentLineWidth += spaceWidth + wordWidth;
                continue;
            }

            // break the line and start the next line with the current word
            this->lines.emplace_back(currentLine, currentLineWidth);
            currentLine = word.second;
            currentLineWidth = wordWidth;
        }

        this->lines.emplace_back(currentLine, currentLineWidth);
    }
}

std::vector<std::pair<int, std::vector<int>>> TextField::explodeByCodepoint(const std::vector<int>& text, int separator)
{
    std::vector<std::pair<int, std::vector<int>>> result; // pair.first = separator count before string, pair.second = string as codepoints

    std::vector<int> textPart;
    int separatorCount = 0;
    for (int codepoint : text) {
        if (codepoint != separator) {
            textPart.push_back(codepoint);
            continue;
        }
        if (!textPart.empty()) {
            result.emplace_back(separatorCount, textPart);
            textPart.clear();
            separatorCount = 1;
        } else separatorCount++;
    }
    result.emplace_back(separatorCount, textPart);

    return result;
}

std::vector<std::pair<std::vector<int>, float>> TextField::explodeByFieldWidth(const std::vector<int>& text)
{
    std::vector<std::pair<std::vector<int>, float>> result;

    std::vector<int> line;
    float lineWidth = 0;
    for (int i = 0; i < text.size(); i++) {

        int kern = i ? stbtt_GetCodepointKernAdvance(&this->opts.font->info, text[i - 1], text[i]) * this->opts.font->scale : 0;
        float charWidth = this->opts.font->getGlyph(text[i])->advanceWidth * this->opts.font->scale;

        if (lineWidth + kern + charWidth > this->width) {
            result.emplace_back(line, lineWidth);
            line.assign(1, text[i]);
            lineWidth = charWidth;
            continue;
        }

        line.push_back(text[i]);
        lineWidth += kern + charWidth;
    }
    result.emplace_back(line, lineWidth);

    return result;
}

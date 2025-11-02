#pragma once

#include "ui/widget.h"

#include "canvas.h"
#include "draw/text.h"

#include <string>
#include <vector>
#include <utility>  // for std::pair

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

class TextField : public Widget
{
public:
    std::string text;
    Text::Options opts;
    TextAlign textAlign = CENTER_CENTER;

    TextField(float x, float y, float width, float height, const std::string& text, const Text::Options& opts)
        : Widget(x, y, width, height), text(text), opts(opts)
    {}

    void draw(Canvas* canvas) override;

private:
    std::vector<std::pair<std::vector<int>, float>> lines; // pair.first = string as codepoints, pair.second = string width in pixels

    void drawLines(Canvas* canvas);
    void buildLines();
    std::vector<std::pair<int, std::vector<int>>> explodeByCodepoint(const std::vector<int>& text, int separator);
    std::vector<std::pair<std::vector<int>, float>> explodeByFieldWidth(const std::vector<int>& text);
};

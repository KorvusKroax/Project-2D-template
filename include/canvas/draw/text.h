#pragma once

#include "canvas.h"
#include "color/color.h"
#include "font.h"

#include <string>

class Text
{
public:
    struct Options {
        Font* font = nullptr;
        Color textColor = WHITE;
        Color shadowColor = CLEAR; // mehetne egy külön "text effect" osztályba amiből lehetne több is mint pl. outline
        unsigned int tabSize = 4; // space count
        float lineHeightScale = 1.0f; // scalar value
    };

    static void draw(Canvas* canvas, float x, float y, const std::string& text, const Options& opts);
    static void draw_line(Canvas* canvas, float x, float y, const std::vector<int>& codepoints, const Options& opts);
    static void draw_char(Canvas* canvas, float x, float y, int codepoint, const Options& opts, float* advanceWidth = nullptr);

    static int utf8ToCodepoint(const char* utf8char, int* bytesUsed = nullptr);
    static std::vector<int> utf8ToCodepoints(const std::string& str);

    static float calcLineWidth(const std::vector<int>& codepoints, const Options& opts);
    static float calcLineHeight(const Options& opts);
};

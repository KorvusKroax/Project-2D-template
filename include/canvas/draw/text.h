#pragma once

#include "canvas.h"
#include "color/color.h"
#include "ui/font.h"

#include <string>
#include <utility>  // std::pair

class Text
{
public:
    struct Options {
        Font* font = nullptr;
        Color color = WHITE;
        unsigned int tabSize = 4; // space count
        float lineHeightScale = 1.0f; // scalar value

        Color shadow_color = CLEAR;
        std::pair<int, int> shadow_direction{1, -1};

        Color outline_color = CLEAR;
        int outline_width = 0;
    };

    static void draw(Canvas* canvas, float x, float y, const std::string& text, const Options& opts);
    static void draw_line(Canvas* canvas, float x, float y, const std::vector<int>& codepoints, const Options& opts);
    static void draw_char(Canvas* canvas, float x, float y, int codepoint, const Options& opts, float* advanceWidth = nullptr);
    static void addGlyph(Canvas* canvas, float x, float y, Glyph* glyph, const Options& opts);
    static void addShadow(Canvas* canvas, float x, float y, Glyph* glyph, const Options& opts);
    static void addOutline(Canvas* canvas, float x, float y, Glyph* glyph, const Options& opts);

    static int utf8ToCodepoint(const char* utf8char, int* bytesUsed = nullptr);
    static std::vector<int> utf8ToCodepoints(const std::string& str);

    static float calcLineWidth(const std::vector<int>& codepoints, const Options& opts); // it can't handle tabs and line breaks !!!
    static float calcLineHeight(const Options& opts);
};

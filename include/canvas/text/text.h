#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"

#include <string>
#include <vector>

struct Text
{
    struct RenderOptions {
        Font* font;
        Color textColor, shadowColor;
        unsigned int tabSize; // space count
        float lineSpacing; // scalar

        RenderOptions(Font* font, Color textColor = WHITE, Color shadowColor = CLEAR, unsigned int tabSize = 4, float lineSpacing = 1.0f)
            : font(font), textColor(textColor), shadowColor(shadowColor), tabSize(tabSize), lineSpacing(lineSpacing)
        {}
    };

    static float draw_char(Canvas* canvas, float x, float y, const char* ch, const RenderOptions &opts)
    {
        return draw_char(canvas, x, y, opts.font->utf8ToCodepoint(ch), opts);
    }

    static float draw_char(Canvas* canvas, float x, float y, int codepoint, const RenderOptions &opts)
    {
        if (codepoint == '\t') {
            Glyph* glyph = opts.font->getGlyph(' ');
            return (glyph->advanceWidth * 4 + stbtt_GetCodepointKernAdvance(&opts.font->info, ' ', ' ') * 3) * opts.font->scale;
        }

        Glyph* glyph = opts.font->getGlyph(codepoint);

        Color textColor = opts.textColor;
        if (glyph->width > 0 && glyph->height > 0) {
            for (int j = 0; j < glyph->height; j++) {
                bool setted = false;
                for (int i = 0; i < glyph->width; i++) {
                    textColor.a = glyph->bitmap[j * glyph->width + i];
                    if (textColor.a == 0) {
                        setted = false;
                        continue;
                    }

                    int px = (int)std::lround(x + glyph->x0 + i);
                    int py = (int)std::lround((y  - glyph->y0) - j);
                    canvas->setPixel(px, py, textColor);

                    if (opts.shadowColor.a > 0) {
                        canvas->setPixel(px + 1, py - 1, opts.shadowColor);
                        if (!setted) canvas->setPixel(px, py - 1, opts.shadowColor);
                        setted = true;
                    }
                }
            }
        }

        return glyph->advanceWidth * opts.font->scale;
    }

    static void draw_line(Canvas* canvas, float x, float y, const std::string &text, const RenderOptions &opts)
    {
        draw_line(canvas, x, y, opts.font->utf8ToCodepoints(text), opts);
    }

    static void draw_line(Canvas* canvas, float x, float y, const std::vector<int> &codepoints, const RenderOptions &opts)
    {
        for (int i = 0; i < codepoints.size(); i++) {
            x += draw_char(canvas, x, y, codepoints[i], opts);
            if (i + 1 < codepoints.size()) {
                x += stbtt_GetCodepointKernAdvance(&opts.font->info, codepoints[i], codepoints[i + 1]) * opts.font->scale;
            }
        }
    }

    static void draw_multiline(Canvas* canvas, float x, float y, const std::string &text, const RenderOptions &opts)
    {
        float lineHeight = (opts.font->ascent - opts.font->descent + opts.font->lineGap) * opts.font->scale * opts.lineSpacing;

        int start = 0;
        float yPos = y;
        while (start < text.size()) {
            int end = text.find('\n', start);
            draw_line(canvas, x, yPos, text.substr(start, end - start), opts);
            yPos -= lineHeight;
            start = (end == std::string::npos) ? text.size() : end + 1;
        }
    }
};

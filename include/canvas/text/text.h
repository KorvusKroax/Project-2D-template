#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"

struct Text
{
    static void draw_char(Canvas* canvas, Font* font, int codepoint, float* x, int y, Color color)
    {
        Glyph* glyph = font->getGlyph(codepoint);

        if (glyph->width > 0 && glyph->height > 0) {
            for (int j = 0; j < glyph->height; j++) {
                for (int i = 0; i < glyph->width; i++) {
                    color.a = glyph->bitmap[j * glyph->width + i];
                    if (color.a == 0) continue;

                    int px = (int)std::lround(*x + glyph->x0 + i);
                    int py = (y  - glyph->y0) - j;

                    canvas->setPixel(px, py, color);
                }
            }
        }
        *x += glyph->advanceWidth * font->scale;
    }

    static void draw_line(Canvas* canvas, Font* font, const std::string text, int x, int y, Color color, float lineSpacingMultiplier = 1.0f)
    {
        float lineHeight = (font->ascent - font->descent + font->lineGap) * font->scale * lineSpacingMultiplier;

        int start = 0;
        int yPos = y;
        while (start < text.size()) {
            int end = text.find('\n', start);
            std::vector<int> codepoints = font->utf8_to_codepoints(text.substr(start, end - start));

            float xPos = x;
            for (int i = 0; i < codepoints.size(); i++) {
                draw_char(canvas, font, codepoints[i], &xPos, yPos, color);
                if (i + 1 < codepoints.size()) {
                    xPos += stbtt_GetGlyphKernAdvance(&font->info, codepoints[i], codepoints[i + 1]) * font->scale;
                }
            }
            yPos -= lineHeight;
            start = (end == std::string::npos) ? text.size() : end + 1;
        }
    }
};

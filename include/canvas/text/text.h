#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"

struct Text
{
    static void draw_char(Canvas* canvas, Font* font, int codepoint, float* x, int y, Color color, Color shadow = CLEAR)
    {
        Glyph* glyph = font->getGlyph(codepoint);

        if (glyph->width > 0 && glyph->height > 0) {
            for (int j = 0; j < glyph->height; j++) {
                bool setted = false;
                for (int i = 0; i < glyph->width; i++) {
                    color.a = glyph->bitmap[j * glyph->width + i];
                    if (color.a == 0) {
                        setted = false;
                        continue;
                    }

                    int px = (int)std::lround(*x + glyph->x0 + i);
                    int py = (y  - glyph->y0) - j;
                    canvas->setPixel(px, py, color);

                    if (shadow.a > 0) {
                        // shadow.a = (unsigned char)(float(shadow.a * (color.a / 255.0f)));
                        canvas->setPixel(px + 1, py - 1, shadow);
                        if (!setted) canvas->setPixel(px, py - 1, shadow);
                        setted = true;
                    }
                }
            }
        }
        *x += glyph->advanceWidth * font->scale;
    }

    static void draw_line(Canvas* canvas, Font* font, const std::string text, int x, int y, Color color, Color shadow = CLEAR, float lineSpacingMultiplier = 1.0f)
    {
        float lineHeight = (font->ascent - font->descent + font->lineGap) * font->scale * lineSpacingMultiplier;

        int start = 0;
        int yPos = y;
        while (start < text.size()) {
            int end = text.find('\n', start);
            std::vector<int> codepoints = font->utf8_to_codepoints(text.substr(start, end - start));

            float xPos = x;
            for (int i = 0; i < codepoints.size(); i++) {
                draw_char(canvas, font, codepoints[i], &xPos, yPos, color, shadow);
                if (i + 1 < codepoints.size()) {
                    xPos += stbtt_GetGlyphKernAdvance(&font->info, codepoints[i], codepoints[i + 1]) * font->scale;
                }
            }
            yPos -= lineHeight;
            start = (end == std::string::npos) ? text.size() : end + 1;
        }
    }
};

#pragma once

#include "canvas.h"
#include "font.h"
#include "color.h"

#include <string>
#include <vector>

struct Text
{
    static float draw_char(Canvas *canvas, float x, float y, const char *ch, Font *font, Color color, Color shadow = CLEAR, unsigned int tabSize = 4)
    {
        return draw_char(canvas, x, y, font->utf8ToCodepoint(ch), font, color, shadow, tabSize);
    }

    static float draw_char(Canvas *canvas, float x, float y, int codepoint, Font *font, Color color, Color shadow = CLEAR, unsigned int tabSize = 4)
    {
        if (codepoint == '\t') {
            Glyph *glyph = font->getGlyph(' ');
            return (glyph->advanceWidth * 4 + stbtt_GetCodepointKernAdvance(&font->info, ' ', ' ') * 3) * font->scale;
        }

        Glyph *glyph = font->getGlyph(codepoint);

        if (glyph->width > 0 && glyph->height > 0) {
            for (int j = 0; j < glyph->height; j++) {
                bool setted = false;
                for (int i = 0; i < glyph->width; i++) {
                    color.a = glyph->bitmap[j * glyph->width + i];
                    if (color.a == 0) {
                        setted = false;
                        continue;
                    }

                    int px = (int)std::lround(x + glyph->x0 + i);
                    int py = (int)std::lround((y  - glyph->y0) - j);
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

        return glyph->advanceWidth * font->scale;
    }

    static void draw_line(Canvas *canvas, float x, float y, const std::string text, Font *font, Color color, Color shadow = CLEAR, unsigned int tabSize = 4)
    {
        draw_line(canvas, x, y, font->utf8ToCodepoints(text), font, color, shadow, tabSize);
    }

    static void draw_line(Canvas *canvas, float x, float y, const std::vector<int> codepoints, Font *font, Color color, Color shadow = CLEAR, unsigned int tabSize = 4)
    {
        for (int i = 0; i < codepoints.size(); i++) {
            x += draw_char(canvas, x, y, codepoints[i], font, color, shadow, tabSize);
            if (i + 1 < codepoints.size()) {
                x += stbtt_GetCodepointKernAdvance(&font->info, codepoints[i], codepoints[i + 1]) * font->scale;
            }
        }
    }

    static void draw_multiline(Canvas *canvas, float x, float y, const std::string text, Font *font, Color color, Color shadow = CLEAR, unsigned int tabSize = 4, float lineSpacingMultiplier = 1.0f)
    {
        float lineHeight = (font->ascent - font->descent + font->lineGap) * font->scale * lineSpacingMultiplier;

        int start = 0;
        float yPos = y;
        while (start < text.size()) {
            int end = text.find('\n', start);
            draw_line(canvas, x, yPos, text.substr(start, end - start), font, color, shadow, tabSize);
            yPos -= lineHeight;
            start = (end == std::string::npos) ? text.size() : end + 1;
        }
    }
};

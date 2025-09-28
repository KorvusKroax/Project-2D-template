#pragma once

#include "canvas.h"
#include "font.h"

#include <vector>

struct Text
{
    static void draw_char(Canvas* canvas, Font* font, int codepoint, int* x, int y, Color color)
    {
        Glyph* glyph = font->getGlyph(codepoint);
        int baseline = (int)(font->ascent * font->scale);

        for (int j = 0; j < glyph->height; j++) {
            for (int i = 0; i < glyph->width; i++) {
                if (glyph->bitmap[i + j * glyph->width] != 0) {
                    canvas->setPixel(
                        *x + glyph->xOffset + i,
                        (canvas->height - 1) - (y + baseline + glyph->yOffset + j),
                        color
                    );
                }
            }
        }

        *x += glyph->advance;
    }

    static void draw_text(Canvas* canvas, Font* font, const std::string text, int x, int y, Color color, int lineHeightOverride = -1)
    {
        int xPos = x;
        int yPos = y;
        int lineHeight = lineHeightOverride > -1 ? lineHeightOverride : int((font->ascent - font->descent + font->lineGap) * font->scale);

        std::vector<int> codepoints = utf8_to_codepoints(text);

        for (int codepoint : codepoints) {
            if (codepoint == '\n') {
                yPos += lineHeight;
                xPos = x;
                continue;
            }
            draw_char(canvas, font, codepoint, &xPos, yPos, color);
        }
    }

private:
    static std::vector<int> utf8_to_codepoints(const std::string text)
    {
        std::vector<int> result;
        for (size_t i = 0; i < text.size();) {
            unsigned char asciiCode = text[i];
            if (asciiCode < 0x80) {
                result.push_back(asciiCode);
            } else if ((asciiCode >> 5) == 0x6) {
                int codepoint = ((asciiCode & 0x1F) << 6) | (text[i + 1] & 0x3F);
                result.push_back(codepoint);
                i++;
            } else if ((asciiCode >> 4) == 0xE) {
                int codepoint = ((asciiCode & 0x0F) << 12) | ((text[i + 1] & 0x3F) << 6) | (text[i + 2] & 0x3F);
                result.push_back(codepoint);
                i += 2;
            }
            i++;
        }

        return result;
    }
};

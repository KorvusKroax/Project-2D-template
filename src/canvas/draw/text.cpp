#include "draw/text.h"

#include "ui/glyph.h"

#include <vector>

void Text::draw(Canvas* canvas, float x, float y, const std::string& text, const Options& opts)
{
    float lineHeight = calcLineHeight(opts);

    int start = 0;
    float yPos = y;
    while (start < text.size()) {
        int end = text.find('\n', start);
        draw_line(canvas, x, yPos, utf8ToCodepoints(text.substr(start, end - start)), opts);
        yPos -= lineHeight;
        start = (end == std::string::npos) ? text.size() : end + 1;
    }
}

void Text::draw_line(Canvas* canvas, float x, float y, const std::vector<int>& codepoints, const Text::Options& opts)
{
    float advanceWidth;
    for (int i = 0; i < codepoints.size(); i++) {
        draw_char(canvas, x, y, codepoints[i], opts, &advanceWidth);
        x += advanceWidth;
        if (i + 1 < codepoints.size()) {
            x += stbtt_GetCodepointKernAdvance(&opts.font->info, codepoints[i], codepoints[i + 1]) * opts.font->scale;
        }
    }
}

void Text::draw_char(Canvas* canvas, float x, float y, int codepoint, const Text::Options& opts, float* advanceWidth)
{
    if (codepoint == '\t') {
        Glyph* spaceGlyph = opts.font->getGlyph(' ');
        if (advanceWidth) *advanceWidth = (spaceGlyph->advanceWidth * opts.tabSize + stbtt_GetCodepointKernAdvance(&opts.font->info, ' ', ' ') * (opts.tabSize - 1)) * opts.font->scale;
        return;
    }

    Glyph* glyph = opts.font->getGlyph(codepoint);

    Color textColor = opts.color;
    if (glyph->width > 0 && glyph->height > 0) {
        for (int j = 0; j < glyph->height; j++) {
            for (int i = 0; i < glyph->width; i++) {
                textColor.a = glyph->bitmap[i + j * glyph->width];
                if (textColor.a == 0) continue;

                int px = (int)x + glyph->x0 + i;
                int py = (int)y - glyph->y0 - j;
                canvas->setPixel(px, py, textColor);
            }
        }
    }

    if (opts.shadow_color.a > 0)
        addShadow(canvas, x, y, glyph, opts);
    else if (opts.outline_color.a > 0)
        addOutline(canvas, x, y, glyph, opts);

    if (advanceWidth) *advanceWidth = glyph->advanceWidth * opts.font->scale;
    return;
}

void Text::addShadow(Canvas* canvas, float x, float y, Glyph* glyph, const Options& opts)
{
    unsigned char alpha;
    Color pixelColor;
    for (int j = 0; j < glyph->height; j++) {
        for (int i = 0; i < glyph->width; i++) {
            alpha = glyph->bitmap[i + j * glyph->width];
            if (alpha == 0) continue;

            int ii = i + opts.shadow_direction.first;
            int jj = j + opts.shadow_direction.second;
            if (ii >= 0 && ii < glyph->width && jj >= 0 && jj < glyph->height &&
                glyph->bitmap[ii + jj * glyph->width] > 0)
                    continue;

            int px = (int)x + glyph->x0 + ii;
            int py = (int)y - glyph->y0 - jj;

            canvas->setPixel(px, py, opts.shadow_color);

            // pixelColor = opts.shadow_color;
            // pixelColor.a *= (float)alpha / 255.0f;
            // canvas->setPixel(px, py, pixelColor);
        }
    }
}

void Text::addOutline(Canvas* canvas, float x, float y, Glyph* glyph, const Options& opts)
{
    unsigned char alpha;
    Color pixelColor;
    for (int j = 0; j < glyph->height; j++) {
        for (int i = 0; i < glyph->width; i++) {
            alpha = glyph->bitmap[i + j * glyph->width];
            if (alpha == 0) continue;

            int px = (int)x + glyph->x0 + i;
            int py = (int)y - glyph->y0 - j;
            for (int l = -opts.outline_size; l <= opts.outline_size; l++) {
                for (int k = -opts.outline_size; k <= opts.outline_size; k++) {
                    if (k == 0 && l == 0) continue;

                    int ii = i + k;
                    int jj = j - l;
                    if (ii >= 0 && ii < glyph->width && jj >= 0 && jj < glyph->height &&
                        glyph->bitmap[ii + jj * glyph->width] > 0)
                            continue;

                    canvas->setPixel(px + k, py + l, opts.outline_color);
                }
            }
        }
    }
}

int Text::utf8ToCodepoint(const char* utf8char, int* bytesUsed)
{
    const unsigned char ch = (unsigned char)utf8char[0];
    int codepoint = 0;
    int length = 0;

    if (ch < 0x80) {
        codepoint = ch;
        length = 1;
    } else if ((ch >> 5) == 0x6) {
        codepoint = ((ch & 0x1f) << 6) | ((unsigned char)utf8char[1] & 0x3f);
        length = 2;
    } else if ((ch >> 4) == 0xe) {
        codepoint = ((ch & 0x0f) << 12) |
            (((unsigned char)utf8char[1] & 0x3f) << 6) |
            ((unsigned char)utf8char[2] & 0x3f);
        length = 3;
    } else if ((ch >> 3) == 0x1e) {
        codepoint = ((ch & 0x07) << 18) |
            (((unsigned char)utf8char[1] & 0x3f) << 12) |
            (((unsigned char)utf8char[2] & 0x3f) << 6) |
            ((unsigned char)utf8char[3] & 0x3f);
        length = 4;
    } else {
        codepoint = 0xfffd;
        length = 1;
    }

    if (bytesUsed) *bytesUsed = length;
    return codepoint;
}

std::vector<int> Text::utf8ToCodepoints(const std::string& str)
{
    std::vector<int> result;
    int i = 0;

    while (i < str.size()) {
        int used = 0;
        int cp = utf8ToCodepoint(&str[i], &used);

        if (used == 0) break;

        result.push_back(cp);
        i += used;
    }

    return result;
}

// it can't handle tabs and line breaks !!!
float Text::calcLineWidth(const std::vector<int>& codepoints, const Options& opts)
{
    float textWidth = 0;
    for (int i = 0; i < codepoints.size(); i++) {
        textWidth +=
            (i ? stbtt_GetCodepointKernAdvance(&opts.font->info, codepoints[i - 1], codepoints[i]) : 0) +
            opts.font->getGlyph(codepoints[i])->advanceWidth;
    }

    return textWidth * opts.font->scale;
}

float Text::calcLineHeight(const Options& opts)
{
    return opts.font->calcFontHeight() * opts.lineHeightScale;
}

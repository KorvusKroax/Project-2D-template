#pragma once

#include "canvas.h"
#include "color.h"

struct Rectangle
{
    static bool draw(Canvas* canvas, int x, int y, int width, int height, Color color, int pattern = 0xffffffff)
    {
        if (x + width < 0 || x >= canvas->width || y + height < 0 || y >= canvas->height) return false;

        int p = 0;
        for (int i = 0; i < width; i++) {
            if (((pattern >> ((p++) % 32)) & 1) == 0) continue;
            if (x + i < 0) continue;
            if (x + i >= canvas->width) break;
            if (y + height < canvas->height) canvas->setPixel(x + i, y + height - 1, color);
        }
        for (int j = height - 2; j >= 1; j--) {
            if (((pattern >> ((p++) % 32)) & 1) == 0) continue;
            if (y + j >= canvas->height) continue;
            if (y + j < 0) break;
            if (x >= 0) canvas->setPixel(x, y + j, color);
        }
        for (int i = width - 1; i >= 0; i--) {
            if (((pattern >> ((p++) % 32)) & 1) == 0) continue;
            if (x + i >= canvas->width) continue;
            if (x + i < 0) break;
            if (y >= 0) canvas->setPixel(x + i, y, color);
        }
        for (int j = 1; j < height - 1; j++) {
            if (((pattern >> ((p++) % 32)) & 1) == 0) continue;
            if (y + j < 0) continue;
            if (y + j >= canvas->height) break;
            if (x + width < canvas->width) canvas->setPixel(x + width - 1, y + j, color);
        }
        return true;
    }

    static bool draw_filled(Canvas* canvas, int x, int y, int width, int height, Color color)
    {
        if (x + width < 0 || x >= canvas->width || y + height < 0 || y >= canvas->height) return false;

        for (int j = 0; j < height; j++)  {
            if (y + j < 0) continue;
            if (y + j >= canvas->height) break;
            for (int i = 0; i < width; i++) {
                if (x + i < 0) continue;
                if (x + i >= canvas->width) break;
                canvas->setPixel(x + i, y + j, color);
            }
        }
        return true;
    }
};

#pragma once

#include "canvas.h"
#include "color.h"

#include <vector>

struct Fill
{
    static bool flood(Canvas* canvas, int x, int y, Color color)
    {
        if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) return false;

        Color targetColor, currPixel;
        if (!canvas->getPixel(x, y, &targetColor) || targetColor == color) return false;
        canvas->setPixel(x, y, color);

        int dir[] = {0,1, 1,0, 0,-1, -1,0};

        std::vector<std::pair<int, int>> next;
        while (true) {
            for (int i = 0; i < 8; i += 2) {
                if (canvas->getPixel(x + dir[i], y + dir[i + 1], &currPixel) && currPixel == targetColor) {
                    canvas->setPixel(x + dir[i], y + dir[i + 1], color);
                    next.push_back({x + dir[i], y + dir[i + 1]});
                }
            }
            if (next.empty()) break;

            std::pair<int, int> last = next.back();
            x = last.first;
            y = last.second;
            next.pop_back();
        }
        return true;
    }

    static bool span(Canvas* canvas, int x, int y, Color color)
    {
        if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) return false;

        Color targetColor, currPixel;
        if (!canvas->getPixel(x, y, &targetColor) || targetColor == color) return false;

        struct Quad {int a, b, c, d;};
        std::vector<Quad> next;

        next.push_back({x, x, y, 1});

        int x1 = x;
        int x2 = x;
            y  = y - 1;
        int dy = -1;
        while (true) {
            x = x1;
            if (canvas->getPixel(x, y, &currPixel) && currPixel == targetColor) {
                while (canvas->getPixel(x - 1, y, &currPixel) && currPixel == targetColor) {
                    canvas->setPixel(x - 1, y, color);
                    x--;
                }
                if (x < x1) next.push_back({x, x1 - 1, y - dy, -dy});
            }

            while (x1 <= x2) {
                while (canvas->getPixel(x1, y, &currPixel) && currPixel == targetColor) {
                    canvas->setPixel(x1, y, color);
                    x1++;
                }
                if (x1 > x) next.push_back({x, x1 - 1, y + dy, dy});
                if (x1 - 1 > x2) next.push_back({x2 + 1, x1 - 1, y - dy, -dy});
                x1++;
                while (x1 < x2 && canvas->getPixel(x1, y, &currPixel) && currPixel != targetColor) x1++;
                x = x1;
            }

            if (next.empty()) break;

            Quad quad = next.back();
            x1 = quad.a;
            x2 = quad.b;
            y = quad.c;
            dy = quad.d;
            next.pop_back();
        }

        return true;
    }
};

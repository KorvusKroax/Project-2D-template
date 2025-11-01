#pragma once

#include "canvas.h"
#include "color/color.h"

class Line
{
public:
    static void draw(Canvas* canvas, int x1, int y1, int x2, int y2, Color color, int pattern = 0xffffffff);
    static void draw_AA(Canvas* canvas, int x1, int y1, int x2, int y2, Color color);

    static void setPerpendicular(float* x1, float* y1, float* x2, float* y2);
    static bool getNormalizedDirection(float x1, float y1, float x2, float y2, float* dir_x, float* dir_y);
    static bool intersection_line(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy);
    static bool intersection_segment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* ix, float* iy);
    static bool intersection_ray(float x1, float y1, float x1_dir, float y1_dir, float x2, float y2, float x2_dir, float y2_dir, float* ix, float* iy);
    static void boundIntersection_ray(float start_x, float start_y, float dir_x, float dir_y, float min_x, float min_y, float max_x, float max_y, float* ix, float* iy);
    static bool boundClip_segment(float* x1, float* y1, float* x2, float* y2, float xmin, float ymin, float xmax, float ymax);
};

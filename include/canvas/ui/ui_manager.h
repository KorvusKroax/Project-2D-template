#pragma once

#include "ui/widget.h"

#include "canvas.h"

#include <memory>

class UIManager
{
    std::vector<std::shared_ptr<Widget>> widgets;

public:
    void add(const std::shared_ptr<Widget>& newWidget);
    void draw(Canvas* canvas);
    void updateHover(float mx, float my);
    void handleClick(float mx, float my);
};

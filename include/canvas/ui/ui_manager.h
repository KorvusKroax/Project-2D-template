#pragma once

#include "widget.h"

#include "canvas.h"

#include <memory>

class UIManager
{
    std::vector<std::shared_ptr<Widget>> widgets;

public:
    void add(const std::shared_ptr<Widget>& newWidget)
    {
        widgets.push_back(newWidget);
    }

    void draw(Canvas* canvas)
    {
        for (std::shared_ptr<Widget>& widget : widgets) {
            widget->draw(canvas);
        }
    }

    void updateHover(float mx, float my)
    {
        for (std::shared_ptr<Widget>& widget : widgets) {
            widget->updateHover(mx, my);
        }
    }

    void handleClick(float mx, float my)
    {
        for (std::shared_ptr<Widget>& widget : widgets) {
            if (widget->hovered) widget->onClick();
        }
    }
};

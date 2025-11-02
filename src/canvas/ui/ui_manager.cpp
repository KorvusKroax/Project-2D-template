#include "ui/ui_manager.h"

#include "ui/widget.h"
#include "canvas.h"

#include <memory>

void UIManager::add(const std::shared_ptr<Widget>& newWidget)
{
    widgets.push_back(newWidget);
}

void UIManager::draw(Canvas* canvas)
{
    for (std::shared_ptr<Widget>& widget : this->widgets) {
        widget->draw(canvas);
    }
}

void UIManager::updateHover(float mx, float my)
{
    for (std::shared_ptr<Widget>& widget : this->widgets) {
        widget->updateHover(mx, my);
    }
}

void UIManager::handleClick(float mx, float my)
{
    for (std::shared_ptr<Widget>& widget : this->widgets) {
        if (widget->hovered) widget->onClick();
    }
}

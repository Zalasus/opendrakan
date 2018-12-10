/*
 * ContainerWidget.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include <odCore/gui/ContainerWidget.h>

namespace odGui
{

    ContainerWidget::ContainerWidget(GuiManager &gm)
    : Widget(gm)
    {
    }

    void ContainerWidget::addWidget(Widget *w)
    {
        mChildWidgets.push_back(od::RefPtr<Widget>(w));

        w->setParent(this);
    }

    void ContainerWidget::intersectChildren(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
        for(auto it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it)
        {
            (*it)->intersect(pointNdc, parentMatrix, parentInverseMatrix, hitWidgets);
        }
    }

}

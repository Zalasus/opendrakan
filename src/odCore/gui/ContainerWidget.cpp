/*
 * ContainerWidget.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include <odCore/gui/ContainerWidget.h>

#include <algorithm>

namespace odGui
{

    ContainerWidget::ContainerWidget()
    {
    }

    void ContainerWidget::addWidget(Widget *w)
    {
        if(w == nullptr)
        {
            return;
        }

        mChildWidgets.push_back(od::RefPtr<Widget>(w));

        w->setParent(this);
    }

    void ContainerWidget::removeWidget(Widget *w)
    {
        if(w == nullptr)
        {
            return;
        }

        auto it = std::find(mChildWidgets.begin(), mChildWidgets.end(), w);
        if(it != mChildWidgets.end())
        {
            mChildWidgets.erase(it);
        }

        w->setParent(nullptr);
    }

    void ContainerWidget::intersectChildren(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
        for(auto it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it)
        {
            (*it)->intersect(pointNdc, parentMatrix, parentInverseMatrix, hitWidgets);
        }
    }

}

/*
 * DrawableWidget.cpp
 *
 *  Created on: 14 Dec 2018
 *      Author: zal
 */

#include <odCore/gui/DrawableWidget.h>

#include <algorithm>

#include <odCore/render/GuiQuad.h>

namespace odGui
{

    DrawableWidget::~DrawableWidget()
    {
    }

    void DrawableWidget::addDrawable(odRender::GuiQuad *quad)
    {
        mDrawables.emplace_back(quad);
    }

    void DrawableWidget::removeDrawable(odRender::GuiQuad *quad)
    {
        auto it = std::find(mDrawables.begin(), mDrawables.end(), quad);
        if(it != mDrawables.end())
        {
            mDrawables.erase(it);
        }
    }

    void DrawableWidget::flattenDrawables(const glm::mat4 &parentMatrix)
    {
        glm::mat4 myMatrix = getParentToWidgetSpaceMatrix() * parentMatrix;

        for(auto &&d : mDrawables)
        {
            d->setMatrix(myMatrix);
        }
    }


}

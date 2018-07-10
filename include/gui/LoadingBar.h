/*
 * LoadingBar.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_LOADINGBAR_H_
#define INCLUDE_GUI_LOADINGBAR_H_

#include "gui/TexturedQuad.h"
#include "gui/DrawableWidget.h"

namespace od
{

    class GuiManager;

    class LoadingBar : public DrawableWidget
    {
    public:

        LoadingBar(GuiManager &gm);

        virtual WidgetDimensionType getDimensionType() const override;
        virtual osg::Vec2 getDimensions() const override;

        void setProgress(float f);


    private:

        float mProgress;
        osg::ref_ptr<TexturedQuad> mForegroundQuad;
        osg::ref_ptr<TexturedQuad> mBarQuad;

    };

}

#endif /* INCLUDE_GUI_LOADINGBAR_H_ */

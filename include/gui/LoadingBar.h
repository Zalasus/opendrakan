/*
 * LoadingBar.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_LOADINGBAR_H_
#define INCLUDE_GUI_LOADINGBAR_H_

#include "gui/TexturedQuad.h"
#include "gui/Widget.h"

namespace od
{

    class GuiManager;

    class LoadingBar : public Widget
    {
    public:

        LoadingBar(GuiManager &gm);

        virtual osg::Vec2 getFullScaleDimensions() const override;

        void setProgress(float f);


    private:

        float mProgress;
        osg::ref_ptr<TexturedQuad> mForegroundQuad;
        osg::ref_ptr<TexturedQuad> mBarQuad;

    };

}

#endif /* INCLUDE_GUI_LOADINGBAR_H_ */

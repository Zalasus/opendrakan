/*
 * LoadingBar.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_LOADINGBAR_H_
#define INCLUDE_GUI_LOADINGBAR_H_

#include <odCore/gui/TexturedQuad.h>
#include <odCore/gui/Widget.h>

namespace od
{

    class GuiManager;

    class LoadingBar : public Widget
    {
    public:

        LoadingBar(GuiManager &gm);

        void setProgress(float f);


    private:

        float mProgress;
        osg::ref_ptr<TexturedQuad> mForegroundQuad;
        osg::ref_ptr<TexturedQuad> mBarQuad;

    };

}

#endif /* INCLUDE_GUI_LOADINGBAR_H_ */

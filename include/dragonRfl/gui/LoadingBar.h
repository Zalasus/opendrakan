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

namespace dragonRfl
{

    class LoadingBar : public odGui::Widget
    {
    public:

        LoadingBar(odGui::GuiManager &gm);

        void setProgress(float f);


    private:

        float mProgress;
        osg::ref_ptr<odGui::TexturedQuad> mForegroundQuad;
        osg::ref_ptr<odGui::TexturedQuad> mBarQuad;

    };

}

#endif /* INCLUDE_GUI_LOADINGBAR_H_ */

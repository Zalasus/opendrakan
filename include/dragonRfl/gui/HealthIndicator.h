/*
 * HealthIndicator.h
 *
 *  Created on: Aug 7, 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_HEALTHINDICATOR_H_
#define INCLUDE_GUI_HEALTHINDICATOR_H_

#include <odCore/gui/Widget.h>
#include <odCore/gui/TexturedQuad.h>

namespace dragonRfl
{

    class HealthIndicator : public odGui::Widget
    {
    public:

        HealthIndicator(odGui::GuiManager &gm);

        void playFlashAnim();
        void setHealth(float health);


    private:

        osg::ref_ptr<odGui::TexturedQuad> mOrbContentQuad;
        osg::ref_ptr<odGui::TexturedQuad> mOrbFlashQuad;
        float mHealth;
    };

}


#endif /* INCLUDE_GUI_HEALTHINDICATOR_H_ */

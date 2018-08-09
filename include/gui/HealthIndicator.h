/*
 * HealthIndicator.h
 *
 *  Created on: Aug 7, 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_HEALTHINDICATOR_H_
#define INCLUDE_GUI_HEALTHINDICATOR_H_

#include "gui/Widget.h"
#include "gui/TexturedQuad.h"

namespace od
{

    class HealthIndicator : public Widget
    {
    public:

        HealthIndicator(GuiManager &gm);

        void playFlashAnim();
        void setHealth(float health);


    private:

        osg::ref_ptr<TexturedQuad> mOrbContentQuad;
        osg::ref_ptr<TexturedQuad> mOrbFlashQuad;
        float mHealth;
    };

}


#endif /* INCLUDE_GUI_HEALTHINDICATOR_H_ */

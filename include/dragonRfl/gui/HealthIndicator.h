/*
 * HealthIndicator.h
 *
 *  Created on: Aug 26, 2019
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_
#define INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_

#include <odCore/gui/Widget.h>

#include <odCore/render/GuiQuad.h>

#include <odCore/anim/Interpolator.h>

namespace dragonRfl
{

    class DragonGui;
    class Bubble;

    class HealthIndicator : public odGui::Widget
    {
    public:

        HealthIndicator(DragonGui &gui);
        virtual ~HealthIndicator();

        virtual void onUpdate(float relTime) override;


    private:

        DragonGui &mDragonGui;

        odAnim::Interpolated<float> mHealthLevel;

        od::RefPtr<Bubble> mHealthBubble;
        od::RefPtr<Bubble> mFlashBubble;
    };

}

#endif /* INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_ */

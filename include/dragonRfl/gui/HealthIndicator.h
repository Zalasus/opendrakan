/*
 * HealthIndicator.h
 *
 *  Created on: Aug 26, 2019
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_
#define INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_

#include <odCore/gui/Widget.h>

#include <odCore/anim/Interpolator.h>

namespace dragonRfl
{

    class DragonGui;
    class Orb;

    class HealthIndicator : public odGui::Widget
    {
    public:

        static constexpr float DEFAULT_ANIM_TIME = 0.8;

        HealthIndicator(DragonGui &gui);
        virtual ~HealthIndicator();

        virtual void onUpdate(float relTime) override;

        /**
         * @brief Sets the health orb's fill state (0=empty, 1=full).
         *
         * The transition will be animated in respect to the \c animTime parameter. The animation
         * includes a "flash" of the orb.
         *
         * @param state  The fill state to set the widget to. 0 means empty, 1 means full
         * @param animTime  Duration of the transition animation. Pass 0 to instantly apply new state.
         */
        void setFillState(float state, float animTime = DEFAULT_ANIM_TIME);


    private:

        DragonGui &mDragonGui;

        odAnim::Interpolated<float, odAnim::SineInterpolator<float>> mHealthLevel;

        std::shared_ptr<Orb> mHealthOrb;
        std::shared_ptr<Orb> mFlashOrb;
    };

}

#endif /* INCLUDE_DRAGONRFL_GUI_HEALTHINDICATOR_H_ */

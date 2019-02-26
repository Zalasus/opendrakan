/*
 * LightReceiver.h
 *
 *  Created on: Feb 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_LIGHTRECEIVER_H_
#define INCLUDE_ODCORE_LIGHTRECEIVER_H_

namespace odPhysics
{
    class PhysicsSystem;
    class Handle;
}

namespace od
{

    class Light;

    class LightReceiver
    {
    public:

        virtual ~LightReceiver() = default;

        virtual void removeAffectingLight(od::Light *light) = 0;

        virtual void addAffectingLight(od::Light *light) = 0;

        virtual void clearLightList() = 0;

        void updateAffectingLights(odPhysics::PhysicsSystem &ps, odPhysics::Handle *handle);

    };

}


#endif /* INCLUDE_ODCORE_LIGHTRECEIVER_H_ */

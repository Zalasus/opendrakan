/*
 * LightReceiver.h
 *
 *  Created on: Feb 24, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_LIGHTRECEIVER_H_
#define INCLUDE_ODCORE_LIGHTRECEIVER_H_

#include <vector>

namespace od
{

    class Light;

    class LightReceiver
    {
    public:


        /**
         * Called by a light that had to update it's affection
         */
        void addAffectingLight(od::Light &light);

        void update(odPhysics::Handle *handle, odPhysics::PhysicsSystem &physicsSystem);



    };

}


#endif /* INCLUDE_ODCORE_LIGHTRECEIVER_H_ */

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

        void update(odPhysics::Handle *handle, odPhysics::PhysicsSystem &physicsSystem);



    };

}


#endif /* INCLUDE_ODCORE_LIGHTRECEIVER_H_ */

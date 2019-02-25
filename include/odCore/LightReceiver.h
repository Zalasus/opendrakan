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

        virtual ~LightReceiver() = default;

        virtual void removeAffectingLight(od::Light *light) = 0;

        virtual void addAffectingLight(od::Light *light) = 0;


    };

}


#endif /* INCLUDE_ODCORE_LIGHTRECEIVER_H_ */

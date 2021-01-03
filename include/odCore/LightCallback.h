/*
 * LightCallback.h
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_LIGHTCALLBACK_H_
#define INCLUDE_ODCORE_LIGHTCALLBACK_H_

#include <memory>

namespace od
{
    class Light;

    class LightCallback
    {
    public:

        virtual ~LightCallback() = default;

        virtual void removeAffectingLight(std::shared_ptr<od::Light> light) = 0;
        virtual void addAffectingLight(std::shared_ptr<od::Light> light) = 0;
        virtual void clearLightList() = 0;

    };

}


#endif /* INCLUDE_ODCORE_LIGHTCALLBACK_H_ */

/*
 * LightCallback.h
 *
 *  Created on: Apr 5, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_LIGHTCALLBACK_H_
#define INCLUDE_ODCORE_LIGHTCALLBACK_H_


namespace od
{
    class Light;

    class LightCallback
    {
    public:

        virtual ~LightCallback() = default;

        virtual void removeAffectingLight(od::Light *light) = 0;
        virtual void addAffectingLight(od::Light *light) = 0;
        virtual void clearLightList() = 0;

    };

}


#endif /* INCLUDE_ODCORE_LIGHTCALLBACK_H_ */

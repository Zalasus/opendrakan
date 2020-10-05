/*
 * FrameListener.h
 *
 *  Created on: May 16, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_FRAMELISTENER_H_
#define INCLUDE_ODCORE_RENDER_FRAMELISTENER_H_

#include <odCore/CTypes.h>

namespace odRender
{

    class FrameListener
    {
    public:

        virtual ~FrameListener() = default;

        virtual void onFrameUpdate(double simTime, double relTime, uint32_t frameNumber) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_FRAMELISTENER_H_ */

/*
 * RendererEventListener.h
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_RENDEREREVENTLISTENER_H_
#define INCLUDE_ODCORE_RENDER_RENDEREREVENTLISTENER_H_

namespace odRender
{

    class RendererEventListener
    {
    public:

        virtual ~RendererEventListener() = default;

        virtual void onRenderWindowClosed() = 0;

    };

}

#endif /* INCLUDE_ODCORE_RENDER_RENDEREREVENTLISTENER_H_ */

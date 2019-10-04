/*
 * @file Client.h
 * @date 30 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_CLIENT_H_
#define INCLUDE_ODCORE_CLIENT_H_

namespace odRender
{
    class Renderer;
}

namespace od
{

    class Client
    {
    public:

        Client(odRender::Renderer &renderer);

        inline odRender::Renderer &getRenderer() { return mRenderer; }


    private:

        odRender::Renderer &mRenderer;

    };

}

#endif /* INCLUDE_ODCORE_CLIENT_H_ */

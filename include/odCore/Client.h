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

        inline odRender::Renderer &getRenderer() { return mRenderer; }


    private:



    };

}

#endif /* INCLUDE_ODCORE_CLIENT_H_ */

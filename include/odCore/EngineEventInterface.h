/*
 * EngineEventInterface.h
 *
 *  Created on: Nov 29, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_ENGINEEVENTINTERFACE_H_
#define INCLUDE_ODCORE_ENGINEEVENTINTERFACE_H_


namespace od
{

    class EngineEventInterface
    {
    public:

        virtual ~EngineEventInterface() = default;

        virtual void shutdown() = 0;
        virtual void key


    };

}


#endif /* INCLUDE_ODCORE_ENGINEEVENTINTERFACE_H_ */

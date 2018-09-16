/*
 * RflManager.h
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_RFLMANAGER_H_
#define INCLUDE_ODCORE_RFL_RFLMANAGER_H_

#include <vector>

namespace od
{

    class Engine;
    class RflRegistrar;

    class RflManager
    {
    public:

        friend class RflRegistrar;

        RflManager(Engine &e);



    private:

        static std::vector<RflRegistrar*> &getRflRegistrarListSingleton();

        Engine &mEngine;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

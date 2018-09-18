/*
 * RflManager.h
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_RFLMANAGER_H_
#define INCLUDE_ODCORE_RFL_RFLMANAGER_H_

#include <vector>
#include <memory>
#include <string>

namespace od
{

    class Engine;
    class Rfl;
    class RflRegistrar;

    class RflManager
    {
    public:

        friend class RflRegistrar;

        RflManager(Engine &e);

        Rfl *getRfl(const std::string &name);



    private:

        Engine &mEngine;
        std::vector<std::unique_ptr<Rfl>> mLoadedRfls;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

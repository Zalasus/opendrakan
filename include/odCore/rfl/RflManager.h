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

#include <odCore/rfl/RflEventInterface.h>

namespace od
{
    class Engine;
}

namespace odRfl
{
    class Rfl;
    class RflRegistrar;

    class RflManager : public RflEventInterface
    {
    public:

        RflManager(od::Engine &e);

        Rfl *getRfl(const std::string &name);

        virtual void onStartup() override;


    private:

        od::Engine &mEngine;
        std::vector<std::unique_ptr<Rfl>> mLoadedRfls;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

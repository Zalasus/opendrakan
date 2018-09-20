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
    class Rfl;
    class RflRegistrar;

    class RflManager : public RflEventInterface
    {
    public:

        RflManager(Engine &e);

        Rfl *getRfl(const std::string &name);

        virtual void onStartup() override;
        virtual void onMenuToggle(bool newMode) override;


    private:

        Engine &mEngine;
        std::vector<std::unique_ptr<Rfl>> mLoadedRfls;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

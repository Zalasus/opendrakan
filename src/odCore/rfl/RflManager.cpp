/*
 * RflManager.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#include <odCore/rfl/RflManager.h>

#include <odCore/rfl/Rfl.h>

namespace od
{

    RflManager::RflManager(Engine &engine)
    : mEngine(engine)
    {
        // first, instantiate all statically linked RFLs
        std::vector<RflRegistrar*> &rfls = getRflRegistrarListSingleton();
        mLoadedRfls.reserve(rfls.size());
        for(auto it = rfls.begin(); it != rfls.end(); ++it)
        {
            std::unique_ptr<Rfl> rfl = (*it)->createInstance(mEngine);
            mLoadedRfls.push_back(std::move(rfl));

            Logger::info() << "Loaded RFL " << mLoadedRfls.back()->getName();
        }

        Logger::info() << "Loaded " << mLoadedRfls.size() << " RFL(s)";
    }

    static std::vector<RflRegistrar*> &RflManager::getRflRegistrarListSingleton()
    {
        static std::vector<RflRegistrar*> list(8);

        return list;
    }

}



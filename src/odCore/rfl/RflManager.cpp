/*
 * RflManager.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#include <odCore/rfl/RflManager.h>

#include <algorithm>

#include <odCore/rfl/Rfl.h>
#include <odCore/Exception.h>
#include <odCore/StringUtils.h>

namespace od
{

    RflManager::RflManager(Engine &engine)
    : mEngine(engine)
    {
        // first, instantiate all statically linked RFLs
        std::vector<RflRegistrar*> &rfls = RflRegistrar::getRflRegistrarListSingleton();
        mLoadedRfls.reserve(rfls.size());
        for(auto it = rfls.begin(); it != rfls.end(); ++it)
        {
            std::unique_ptr<Rfl> rfl((*it)->createInstance(mEngine));
            mLoadedRfls.push_back(std::move(rfl));

            Logger::info() << "Loaded RFL " << mLoadedRfls.back()->getName();
        }

        Logger::info() << "Loaded " << mLoadedRfls.size() << " RFL(s)";
    }

    Rfl *RflManager::getRfl(const std::string &name)
    {
        auto pred = [&name](std::unique_ptr<Rfl> &rfl){ return StringUtils::compareIgnoringCase(name, rfl->getName()); };
        auto it = std::find_if(mLoadedRfls.begin(), mLoadedRfls.end(), pred);

        if(it == mLoadedRfls.end())
        {
            Logger::error() << "RFL '" << name << "' is not loaded";
            throw NotFoundException("RFL with given name is not loaded");
        }

        return it->get();
    }

    void RflManager::onStartup()
    {
        for(auto it = mLoadedRfls.begin(); it != mLoadedRfls.end(); ++it)
        {
            (*it)->onStartup();
        }
    }

    void RflManager::onMenuToggle(bool newMode)
    {
        for(auto it = mLoadedRfls.begin(); it != mLoadedRfls.end(); ++it)
        {
            (*it)->onMenuToggle(newMode);
        }
    }

}



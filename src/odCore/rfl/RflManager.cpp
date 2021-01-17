/*
 * RflManager.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#include <odCore/rfl/RflManager.h>

#include <algorithm>

#include <odCore/Logger.h>
#include <odCore/Panic.h>
#include <odCore/StringUtils.h>

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

    RflManager::RflManager()
    {
    }

    Rfl *RflManager::getRfl(const std::string &name)
    {
        auto pred = [&name](std::unique_ptr<Rfl> &rfl){ return od::StringUtils::compareIgnoringCase(name, rfl->getName()); };
        auto it = std::find_if(mLoadedRfls.begin(), mLoadedRfls.end(), pred);

        if(it == mLoadedRfls.end())
        {
            OD_PANIC() << "RFL '" << name << "' is not loaded";
        }

        return it->get();
    }

    Rfl &RflManager::loadDynamicRfl(const od::FilePath &libPath)
    {
        OD_PANIC() << "Dynamic RFL loading is unimplemented ATM";
    }


    void RflManager::_addRflAndCallLoadHook(std::unique_ptr<Rfl> rfl)
    {
        if(rfl == nullptr) return;

        Logger::info() << "Loaded RFL '" << rfl->getName() << "'";

        mLoadedRfls.push_back(std::move(rfl));
        mLoadedRfls.back()->onLoaded();
    }

}

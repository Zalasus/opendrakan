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
#include <type_traits>

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

        /**
         * @brief Finds a loaded RFL with the given name, ignoring case.
         */
        Rfl *getRfl(const std::string &name);

        virtual void onStartup() override;

        template <typename _Rfl>
        void loadStaticRfl()
        {
            static_assert(std::is_base_of<Rfl, _Rfl>::value, "Template argument to loadStaticRfl must inherit from odRfl::Rfl");

            auto newRfl = std::make_unique<_Rfl>(mEngine);
            _addRflAndCallLoadHook(std::move(newRfl));
        }


    private:

        void _addRflAndCallLoadHook(std::unique_ptr<Rfl> rfl);

        od::Engine &mEngine;
        std::vector<std::unique_ptr<Rfl>> mLoadedRfls;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

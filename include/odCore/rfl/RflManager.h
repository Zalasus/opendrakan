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

#include <odCore/FilePath.h>

namespace od
{
    class Engine;
}

namespace odRfl
{
    class Rfl;
    class RflRegistrar;

    class RflManager
    {
    public:

        RflManager();

        /**
         * @brief Finds a loaded RFL with the given name, ignoring case.
         */
        Rfl *getRfl(const std::string &name);

        /**
         * @brief Loads a statically linked RFL.
         *
         * Pass the RFL class as a template argument.
         *
         * @return The RFL instance that was just loaded.
         */
        template <typename _Rfl>
        Rfl &loadStaticRfl()
        {
            static_assert(std::is_base_of<Rfl, _Rfl>::value, "Template argument to loadStaticRfl must inherit from odRfl::Rfl");

            auto newRfl = std::make_unique<_Rfl>();
            auto &rflRef = *newRfl;
            _addRflAndCallLoadHook(std::move(newRfl));

            return rflRef;
        }

        /**
         * @brief Loads an RFL from a dynamic library.
         * @brief The RFL instance that was just loaded.
         */
        Rfl &loadDynamicRfl(const od::FilePath &libPath);


    private:

        void _addRflAndCallLoadHook(std::unique_ptr<Rfl> rfl);

        std::vector<std::unique_ptr<Rfl>> mLoadedRfls;
    };

}


#endif /* INCLUDE_ODCORE_RFL_RFLMANAGER_H_ */

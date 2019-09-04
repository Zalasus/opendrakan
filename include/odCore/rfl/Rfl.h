/*
 * Rfl.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFL_H_
#define INCLUDE_RFL_RFL_H_

#include <string>
#include <map>

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflManager.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

/**
 * @brief Convenience macro for defining the traits type for an RFL.
 *
 * @note Put this in global scope of the header declaring your RFL. Putting it in a namespace won't work.
 *
 * @param rflName   A string containing the RFL's name
 * @param rfl       The class implementing the RFL
 */
#define OD_DEFINE_RFL_TRAITS(rflName, rfl) \
    namespace odRfl\
    {\
        template<> constexpr const char *odRfl::RflTraits<rfl>::name() { return rflName; }\
    }

namespace od
{
    class Engine;
}

namespace odRfl
{

	class Rfl : public RflEventInterface
	{
	public:

		Rfl(od::Engine &engine);
		virtual ~Rfl() = default;

		inline od::Engine &getEngine() { return mEngine; }

		virtual const char *getName() const = 0;
		virtual size_t getRegisteredClassCount() const = 0;
		virtual RflClassRegistrar *getRegistrarForClass(RflClassId id) = 0;
        virtual RflClass *createInstanceOfClass(RflClassId id) = 0;

		virtual void onStartup() override;

		virtual void onRegisterClasses() = 0;


	private:

		od::Engine &mEngine;

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

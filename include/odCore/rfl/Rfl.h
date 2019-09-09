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
#include <utility>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/RflManager.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

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

		virtual void onLoaded() = 0;
		virtual void onStartup() override;

		ClassRegistrar *getRegistrarForClassId(ClassId id);


	protected:

		template <typename _ClassBase>
		void registerClass()
		{
		    ClassId id = ClassTraits<_ClassBase>::classId();
		    ClassRegistrar &registrar = ClassTraits<_ClassBase>::getRegistrar();

		    mRegisteredClasses.insert(std::make_pair(id, std::ref(registrar)));
		}


	private:

		od::Engine &mEngine;

		std::map<ClassId, std::reference_wrapper<ClassRegistrar>> mRegisteredClasses;

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

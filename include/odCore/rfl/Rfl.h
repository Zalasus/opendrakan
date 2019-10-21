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

#include <odCore/Logger.h>
#include <odCore/Exception.h>

namespace od
{
    class Server;
    class Client;
}

namespace odRfl
{

	class Rfl
	{
	public:

		virtual ~Rfl() = default;

		virtual const char *getName() const = 0;

		/**
		 * @brief Loaded hook. Called right after this RFL has been loaded.
		 *
		 * This is where an RFL should register it's classes.
		 */
		virtual void onLoaded() = 0;

		/**
		 * @brief Startup hook. Called when a game is started.
		 *
		 * During the default startup procedure, the engine provides a local client and server which
		 * allows to load the intro level and to create a GUI. If the player then selects multiplayer, the
		 * local server can be swapped out for a remote one.
		 */
		virtual void onGameStartup(od::Server &localServer, od::Client &localClient) = 0;

		ClassFactory *getFactoryForClassId(ClassId id);


	protected:

		template <typename _ClassBase>
		void registerClass()
		{
		    ClassId id = ClassTraits<_ClassBase>::classId();
		    ClassFactory &factory = ClassTraits<_ClassBase>::getFactory();

		    mRegisteredClasses.insert(std::make_pair(id, std::ref(factory)));
		}


	private:

		std::map<ClassId, std::reference_wrapper<ClassFactory>> mRegisteredClasses;

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

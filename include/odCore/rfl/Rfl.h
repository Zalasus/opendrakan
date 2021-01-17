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
         *
         * The loadIntroLevel flag tells the RFL whether it should load an intro level or not. This is just for
         * performance. If there is an initial level override (e.g. from the command line), the app will load that
         * level regardless of whether the RFL already loaded an intro level. Honoring the flag will prevent
         * unnecessary level loading.
		 */
		virtual void onGameStartup(od::Server &localServer, od::Client &localClient, bool loadIntroLevel) = 0;

        /**
         * @brief Called on the server when a new level was loaded.
         */
        virtual void onLevelLoaded(od::Server &localServer) = 0;


		ClassFactory *getFactoryForClassId(ClassId id);


	protected:

		template <typename _ClassDefinition>
		void registerClass()
		{
		    ClassId id = _ClassDefinition::classId();
		    ClassFactory &factory = _ClassDefinition::getFactory();

		    mRegisteredClasses.insert(std::make_pair(id, std::ref(factory)));
		}


	private:

		std::map<ClassId, std::reference_wrapper<ClassFactory>> mRegisteredClasses;

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

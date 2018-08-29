/*
 * RflClass.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASS_H_
#define INCLUDE_RFL_RFLCLASS_H_

#include "rfl/RflFieldProbe.h"
#include "rfl/RflMessage.h"

namespace od
{
	class LevelObject;
	class Engine;
}

namespace odRfl
{

	class RflClass
	{
	public:

		virtual ~RflClass() = default;

		virtual void probeFields(RflFieldProbe &probe) = 0;

		// TODO: maybe create an "EventHandler" interface for this?

		/**
		 * @brief Called when this RFL class is first instantiated from a database class, but after fields have been filled.
		 *
         * This is an overload of onLoaded(od::LevelObject &obj) for RFL classes that aren't used on level objects.
         */
        virtual void onLoaded(od::Engine &e);

		/**
		 * @brief Called when this RFL class is first instantiated from a database class, but after fields have been filled.
		 *
		 * This overload is only used for RFL classes that are used on level objects.
		 */
		virtual void onLoaded(od::LevelObject &obj);

		virtual void onSpawned(od::LevelObject &obj);
		virtual void onDespawned(od::LevelObject &obj);
		virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime);
		virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message);
		virtual void onMoved(od::LevelObject &obj);
		virtual void onDestroyed(od::LevelObject &obj);
	};

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */

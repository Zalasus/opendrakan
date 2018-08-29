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
		 * Called when this RFL class is first instantiated from a database class, but after fields have been filled.
		 * This may also happen to non-level object classes like materials, in which case \c obj will be nullptr.
		 *
		 * This hook should be used when an RFL class needs to register itself as an interface with the engine, while
		 * calls to the engine that require that a different RFL class already registered itself should be made in the
		 * \c spawned() hook. This should prevent issues arising from scenarios in which an object is loaded after one
		 * that depends on it.
		 */
		virtual void onLoaded(od::Engine &e, od::LevelObject *obj);
		virtual void onSpawned(od::LevelObject &obj);
		virtual void onDespawned(od::LevelObject &obj);
		virtual void onUpdate(od::LevelObject &obj, double simTime, double relTime);
		virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message);
		virtual void onMoved(od::LevelObject &obj);
		virtual void onDestroyed(od::LevelObject &obj);
	};

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */

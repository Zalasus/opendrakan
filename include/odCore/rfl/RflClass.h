/*
 * RflClass.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASS_H_
#define INCLUDE_RFL_RFLCLASS_H_

#include <odCore/rfl/RflFieldProbe.h>
#include <odCore/rfl/RflMessage.h>

namespace od
{
	class LevelObject;
	class Engine;
}

namespace od
{

    typedef uint16_t RflClassId;

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


	class RflClassRegistrar
    {
    public:

        RflClassRegistrar(RflClassId classId, const std::string &className);
        virtual ~RflClassRegistrar();

        virtual RflClass *createInstance() = 0;

        inline RflClassId getClassId() const { return mClassId; }
        inline std::string getClassName() const { return mClassName; }


    protected:

        RflClassId mClassId;
        std::string mClassName;

    };


    template <typename _Rfl, typename _Class>
    class RflClassRegistrarImpl : public RflClassRegistrar
    {
    public:

        RflClassRegistrarImpl(RflClassId typeId, const std::string &typeName)
        : RflClassRegistrar(typeId, typeName)
        {
            std::map<RflClassId, RflClassRegistrar*> &map = _Rfl::getClassRegistrarMapSingleton();

            if(map.find(typeId) != map.end())
            {
                Logger::warn() << "Ignoring double registration of RFL class '" << typeName << "' (ID " << std::hex << typeId << std::dec << ")";

            }else
            {
                map.insert(std::make_pair(typeId, this));
            }
        }

        virtual RflClass *createInstance() override
        {
            Logger::debug() << "Creating instance of RFL class '" << mClassName << "' (" << std::hex << mClassId << std::dec << ")";

            return new _Class();
        }
    };

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */

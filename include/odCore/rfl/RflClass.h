/*
 * RflClass.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASS_H_
#define INCLUDE_RFL_RFLCLASS_H_

#include <odCore/rfl/RflMessage.h>

#define OD_REGISTER_RFL_CLASS(rflName, classId, category, className, cppClass) \
    template <> class RflClassTraits { static constexpr const char *name() { return className; }\
    static constexpr RflClassId classId() { return classId; } }\
    static od::RflClassRegistrarImpl<rflName, classCppClass> sOdRflClassRegistrar_ ## classCppClass;

namespace od
{
    class LevelObject;
	class Engine;
	class FieldProbe;

    typedef uint16_t RflClassId;

	class RflClass
	{
	public:

		virtual ~RflClass() = default;

		virtual void probeFields(FieldProbe &probe) = 0;

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


	template <typename _Class>
	class RflClassTraits
	{
	public:

	    static constexpr const char *name() { return "<invalid RFL class template>"; }
	    static constexpr RflClassId classId() { return 0; }

	};


	class RflClassRegistrar
    {
    public:

        virtual ~RflClassRegistrar() = default;

        virtual RflClass *createInstance() const = 0;
        virtual const char *getClassName() const = 0;
        virtual RflClassId getClassId() const = 0;
    };


    template <typename _Rfl, typename _Class>
    class RflClassRegistrarImpl : public RflClassRegistrar
    {
    public:

        RflClassRegistrarImpl()
        {
            std::map<RflClassId, RflClassRegistrar*> &map = RflClassMapHolder<_Rfl>::getClassRegistrarMapSingleton();

            if(map.find(RflClassTraits<_Class>::classId()) != map.end())
            {
                Logger::warn() << "Ignoring double registration of RFL class '" << RflClassTraits<_Class>::name()
                        << "' (ID " << std::hex << RflClassTraits<_Class>::classId() << std::dec << ")";

            }else
            {
                map.insert(std::make_pair(RflClassTraits<_Class>::classId(), this));
            }
        }

        virtual RflClass *createInstance() const override
        {
            Logger::debug() << "Creating instance of RFL class '" << RflClassTraits<_Class>::name() <<
                    "' (" << std::hex << RflClassTraits<_Class>::classId() << std::dec << ")";

            return new _Class();
        }

        virtual const char *getClassName() const override
        {
            return RflClassTraits<_Class>::name();
        }

        virtual RflClassId getClassId() const override
        {
            return RflClassTraits<_Class>::classId();
        }
    };

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */

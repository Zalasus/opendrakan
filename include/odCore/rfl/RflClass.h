/*
 * RflClass.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASS_H_
#define INCLUDE_RFL_RFLCLASS_H_

#include <map>

#include <odCore/rfl/RflMessage.h>
#include <odCore/rfl/FieldProbe.h>
#include <odCore/Logger.h>

/**
 * @brief Convenience macro for defining the traits type for an RFL class.
 *
 * @note Put this in global scope of the header declaring your RFL class. Putting it in a namespace won't work.
 *
 * @param rfl       The class implementing the RFL this class belongs to
 * @param id        The ID of the RFL class
 * @param category  A string containing the RFL class's category
 * @param className A string containing the RFL class's name
 * @param cppClass  The class implementing the RFL class
 */
#define OD_DEFINE_RFLCLASS_TRAITS(rfl, id, category, className, cppClass) \
    namespace odRfl \
    { \
        template <> constexpr const char *RflClassTraits<cppClass>::rflName() { return #rfl; }\
        template <> constexpr const char *RflClassTraits<cppClass>::name() { return className; }\
        template <> constexpr const char *RflClassTraits<cppClass>::categoryName() { return category; }\
        template <> constexpr RflClassId  RflClassTraits<cppClass>::classId() { return id; }\
    }

/**
 * @brief Convenience macro for defining a static registrar object for an RFL class.
 *
 * @note Put this in any source file (like the one defining your class's methods). Can be placed in any namespace.
 *
 * @param rfl       The class implementing the RFL this class belongs to
 * @param cppClass  The class implementing the RFL class
 */
#define OD_REGISTER_RFLCLASS(rfl, cppClass) \
    static odRfl::RflClassRegistrarImpl<rfl, cppClass> sOdRflClassRegistrar_ ## rfl ## _ ## cppClass;

namespace od
{
    class LevelObject;
	class Engine;
	class Layer;
}

namespace odPhysics
{
    class Handle;
    class PhysicsSystem;
}

namespace odRfl
{
	class Rfl;

    typedef uint16_t RflClassId;

	class RflClass
	{
	public:

		virtual ~RflClass();

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
		virtual void onUpdate(od::LevelObject &obj, float relTime);
		virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message);
		virtual void onMoved(od::LevelObject &obj);
		virtual void onDestroyed(od::LevelObject &obj);
		virtual void onVisibilityChanged();
		virtual void onLayerChanged(od::LevelObject &obj, od::Layer *from, od::Layer *to);

	};


	template <typename _Class>
    struct RflClassTraits
    {
        static constexpr const char *rflName();
        static constexpr const char *name();
        static constexpr const char *categoryName();
        static constexpr RflClassId classId();
    };


	class RflClassRegistrar
    {
    public:

        virtual ~RflClassRegistrar() = default;

        virtual RflClass *createInstance(Rfl *rfl) const = 0;
        virtual const char *getClassName() const = 0;
        virtual const char *getClassCategory() const = 0;
        virtual RflClassId getClassId() const = 0;
    };


	template <typename _Rfl>
    class RflClassMapHolder
    {
    public:

        static std::map<RflClassId, RflClassRegistrar*> &getClassRegistrarMapSingleton()
        {
            static std::map<RflClassId, RflClassRegistrar*> map;

            return map;
        }
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
                        << "' (ID = 0x" << std::hex << RflClassTraits<_Class>::classId() << std::dec << ")";

            }else
            {
                map.insert(std::make_pair(RflClassTraits<_Class>::classId(), this));
            }
        }

        virtual RflClass *createInstance(Rfl *rfl) const override
        {
            Logger::debug() << "Creating instance of RFL class '" << RflClassTraits<_Class>::name() <<
                    "' (ID = 0x" << std::hex << RflClassTraits<_Class>::classId() << std::dec << ")";

            _Rfl *subRfl = static_cast<_Rfl*>(rfl);

            return new _Class(*subRfl);
        }

        virtual const char *getClassName() const override
        {
            return RflClassTraits<_Class>::name();
        }

        virtual const char *getClassCategory() const override
        {
            return RflClassTraits<_Class>::categoryName();
        }

        virtual RflClassId getClassId() const override
        {
            return RflClassTraits<_Class>::classId();
        }
    };

}

#endif /* INCLUDE_RFL_RFLCLASSTYPE_H_ */

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
 * @note Put this in the header declaring your RFL class.
 *
 * @param rfl       The class implementing the RFL this class belongs to
 * @param id        The ID of the RFL class
 * @param category  A string containing the RFL class's category
 * @param className A string containing the RFL class's name
 * @param cppClass  The class implementing the RFL class
 */
#define OD_DEFINE_RFLCLASS_TRAITS(rfl, id, category, className, cppClass) \
    template <> struct RflClassTraits<cppClass> { \
        static constexpr const char *rflName() { return #rfl; }\
        static constexpr const char *name() { return className; }\
        static constexpr const char *categoryName() { return category; }\
        static constexpr RflClassId classId() { return id; } };

/**
 * @brief Convenience macro for defining a static registrar object for an RFL class.
 *
 * @note Put this in any source file (like the one defining your class's methods).
 *
 * @param rfl       The class implementing the RFL this class belongs to
 * @param cppClass  The class implementing the RFL class
 */
#define OD_REGISTER_RFLCLASS(rfl, cppClass) \
    static od::RflClassRegistrarImpl<rfl, cppClass> sOdRflClassRegistrar_ ## rfl ## _ ## cppClass;

namespace od
{
    class LevelObject;
	class Engine;
	class Rfl;

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
        virtual void onLoaded(Engine &e);

		/**
		 * @brief Called when this RFL class is first instantiated from a database class, but after fields have been filled.
		 *
		 * This overload is only used for RFL classes that are used on level objects.
		 */
		virtual void onLoaded(LevelObject &obj);

		virtual void onSpawned(LevelObject &obj);
		virtual void onDespawned(LevelObject &obj);
		virtual void onUpdate(LevelObject &obj, double simTime, double relTime);
		virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message);
		virtual void onMoved(LevelObject &obj);
		virtual void onDestroyed(LevelObject &obj);

	};


	template <typename _Class>
	struct RflClassTraits
	{
	    static constexpr const char *rflName() { return "<mising RFL class traits definition>"; }
	    static constexpr const char *name() { return "<mising RFL class traits definition>"; }
	    static constexpr const char *categoryName() { return "<mising RFL class traits definition>"; }
	    static constexpr RflClassId classId() { return 0; }
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

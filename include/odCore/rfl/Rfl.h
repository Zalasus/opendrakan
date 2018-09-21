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

#define OD_DEFINE_RFL_TRAITS(rflName, rfl) \
    template<> struct RflTraits<rfl> { static constexpr const char *name() { return rflName; } }; \

#define OD_REGISTER_RFL(rfl) \
	static od::RflRegistrarImpl<rfl> sOdRflRegistrar_ ## rfl;

namespace od
{

    class Engine;

	class Rfl : public RflEventInterface
	{
	public:

		Rfl(Engine &engine);
		virtual ~Rfl() = default;

		inline Engine &getEngine() { return mEngine; }

		virtual const char *getName() const = 0;
		virtual RflClassRegistrar *getRflClassRegistrar(RflClassId id) = 0;
		virtual size_t getRegisteredClassCount() const = 0;

		virtual void onStartup() override;
		virtual void onMenuToggle(bool newMode) override;


	private:

		Engine &mEngine;

	};


	template <typename T>
    struct RflTraits
    {

        static constexpr const char *name() { return "<invalid RFL template>"; }

    };


	template <typename _SubRfl>
	class AutoRegisteringRfl : public Rfl
	{
	public:

	    virtual const char *getName() const final override
	    {
	        return RflTraits<_SubRfl>::name();
	    }

	    virtual RflClassRegistrar *getRflClassRegistrar(RflClassId id) final override
        {
	        std::map<RflClassId, RflClassRegistrar*> &map = RflClassMapHolder<_SubRfl>::getClassRegistrarMapSingleton();
            auto it = map.find(id);
            if(it == map.end())
            {
                throw NotFoundException("Class with given ID is not registered in RFL");
            }

            return it->second;
        }

	    virtual size_t getRegisteredClassCount() const final override
	    {
	        return RflClassMapHolder<_SubRfl>::getClassRegistrarMapSingleton().size();
	    }


	protected:

	    AutoRegisteringRfl(Engine &e) : Rfl(e) {}

	};


	class RflRegistrar
	{
	public:

	    friend class RflManager;

        virtual ~RflRegistrar() = default;

        virtual const char *getName() const = 0;
	    virtual Rfl *createInstance(Engine &engine) const = 0;


	protected:

	    static std::vector<RflRegistrar*> &getRflRegistrarListSingleton();

	};


	template <typename _Rfl>
	class RflRegistrarImpl : public RflRegistrar
	{
	public:

	    RflRegistrarImpl()
	    {
            getRflRegistrarListSingleton().push_back(this);
        }

	    virtual const char *getName() const override
	    {
	        return RflTraits<_Rfl>::name();
	    }

	    virtual Rfl *createInstance(Engine &engine) const override
	    {
	        return new _Rfl(engine);
	    }

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

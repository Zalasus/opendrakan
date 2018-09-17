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

#define OD_REGISTER_RFL(rfl) \
    template<> class RflTraits<rfl> { public: static constexpr const char *name() { return #rfl; } }; \
	static od::RflRegistrarImpl<rfl> sOdRflRegistrar_ ## rfl(#rfl);

namespace od
{

    class Engine;

	class Rfl
	{
	public:

		Rfl(Engine &engine);
		virtual ~Rfl() = default;

		inline Engine &getEngine() { return mEngine; }

		virtual const char *getName() const = 0;
		virtual RflClass *createClassInstance(RflClassId id) = 0;
		virtual size_t getRegisteredClassCount() = 0;


	private:

		Engine &mEngine;

	};


	template <typename _Rfl>
    class RflClassMapHolder
    {
    public:

        friend class RflClassRegistrar;
        friend class RflImpl;

    private:

        static std::map<RflClassId, RflClassRegistrar*> &getClassRegistrarMapSingleton()
        {
            std::map<RflClassId, RflClassRegistrar*> map;

            return map;
        }
    };


	template <typename _SubRfl>
	class RflImpl : public Rfl
	{
	public:

	    virtual const char *getName() const override
	    {
	        return RflTraits<_SubRfl>::name();
	    }

	    virtual RflClass *createClassInstance(RflClassId id) override
	    {
	        std::map<RflClassId, RflClassRegistrar*> &map = RflClassMapHolder<_SubRfl>::getClassRegistrarMapSingleton();
	        auto it = map.find(id);
	        if(it == map.end())
	        {
	            throw NotFoundException("Class with given ID is not registered in RFL");
	        }

	        return it->second->createInstance();
	    }

	    virtual size_t getRegisteredClassCount() override
	    {
	        return RflClassMapHolder<_SubRfl>::getClassRegistrarMapSingleton().size();
	    }

	};


    template <typename T>
    class RflTraits
    {
    public:

        static constexpr const char *name() { return "<invalid RFL template>"; }

    };


	class RflRegistrar
	{
	public:

        virtual ~RflRegistrar() = default;

        virtual const char *getName() const = 0;
	    virtual Rfl *createInstance(Engine &engine) const = 0;

	};


	template <typename _Rfl>
	class RflRegistrarImpl : public RflRegistrar
	{
	public:

	    RflRegistrarImpl()
	    {
            RflManager::getRflRegistrarListSingleton().push_back(this);
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

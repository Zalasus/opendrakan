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

#define OD_REGISTER_RFL(rflName) \
	static od::RflRegistrarImpl<rflName> sOdRflRegistrar_ ## rflName(#rflName);

#define OD_REGISTER_RFL_CLASS(rflName, classId, category, className, cppClass) \
    static odRfl::RflClassRegistrarImpl<rflName, classCppClass> sOdRflClassRegistrar_ ## classCppClass(classId, className);

namespace od
{

    class Engine;

	class Rfl
	{
	public:

        friend class RflClassRegistrar;

		Rfl(Engine &e);
		virtual ~Rfl() = default;

		virtual const char *getName() = 0;


	private:

        static std::map<RflClassId, RflClassRegistrar*> &getClassRegistrarMapSingleton();

	};


	class RflRegistrar
	{
	public:

        RflRegistrar(const char *rflName) : mName(rflName) { }


        virtual ~RflRegistrar() = default;

        inline const char *getName() const { return mName; }

	    virtual Rfl *createInstance(Engine &e) = 0;


	private:

	    const char *mName;
	};


	template <typename _Rfl>
	class RflRegistrarImpl : public RflRegistrar
	{
	public:

	    RflRegistrarImpl(const char *rflName)
	    : RflRegistrar(rflName)
	    {
            RflManager::getRflRegistrarListSingleton().push_back(this);
        }

	    virtual Rfl *createInstance(Engine &e) override
	    {
	        return new _Rfl(e);
	    }

	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

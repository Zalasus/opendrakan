/*
 * Rfl.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFL_H_
#define INCLUDE_RFL_RFL_H_

#include <string>
#include <functional>
#include <map>
#include <memory>

#include "rfl/RflClass.h"

#define OD_REGISTER_RFL_CLASS(classId, className, classCppClass) \
	static od::RflClassRegistrarImpl<classCppClass> sOdRflRegistrar_ ## _ ## classCppClass (classId, className);

#define OD_REGISTER_RFL(rflName) \
	od::Rfl::RflNameSetter sRflNameSetter(rflName);

namespace od
{

	typedef uint16_t RflClassId;

	class Rfl;

	class RflClassRegistrar
	{
	public:

		RflClassRegistrar(RflClassId classId, const std::string &className);
		virtual ~RflClassRegistrar();

		virtual std::unique_ptr<RflClass> createClassInstance(RflFieldProbe &probe) = 0;

		inline RflClassId getClassId() const { return mClassId; }
		inline std::string getClassName() const { return mClassName; }


	private:

		RflClassId mClassId;
		std::string mClassName;

	};


	template <typename T>
	class RflClassRegistrarImpl : public RflClassRegistrar
	{
	public:

		RflClassRegistrarImpl(RflClassId typeId, const std::string &typeName)
		: RflClassRegistrar(typeId, typeName)
		{
		}

		virtual std::unique_ptr<RflClass> createClassInstance(RflFieldProbe &probe) override
		{
		    std::unique_ptr<RflClass> instance(new T);
		    instance->probeFields(probe);

		    return instance;
		}
	};


	class Rfl
	{
	public:

		class RflNameSetter
		{
		public:
			RflNameSetter(const std::string &name);
		};

		friend class RflClassRegistrar;


		inline std::string getName() const { return mName; }
		inline void setName(const std::string &name) { mName = name; }
		inline size_t getClassTypeCount() const { return mRegistrarMap.size(); }
		RflClassRegistrar &getClassRegistrarById(RflClassId id);


		static Rfl &getSingleton();


	private:

		std::string mName;
		std::map<RflClassId, std::reference_wrapper<RflClassRegistrar>> mRegistrarMap;
	};

}

#endif /* INCLUDE_RFL_RFL_H_ */

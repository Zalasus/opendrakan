/*
 * RiotFunctionLibrary.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_
#define INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_

#include <string>
#include <functional>
#include <map>

#include "rfl/RflClass.h"

#define OD_REGISTER_RFL_CLASS(classId, className, classCppClass) \
	static od::RflClassRegistrarImpl<classCppClass> sOdRflRegistrar_ ## _ ## classCppClass (classId, className);

#define OD_REGISTER_RFL(rflName) \
	od::RiotFunctionLibrary od::RiotFunctionLibrary::smSingleton(rflName);

namespace od
{

	typedef uint16_t RflClassId;

	class RiotFunctionLibrary;

	class RflClassRegistrar
	{
	public:

		RflClassRegistrar(RflClassId classId, const std::string &className);
		virtual ~RflClassRegistrar();

		virtual RflClass *createClassInstance(RflFieldProbe *probe) = 0;

		inline RflClassId getClassId() const { return mClassId; }
		inline std::string getClassName() const { return mClassName; }


	private:

		RflClassId mClassId;
		std::string mClassName;

	};


	template <typename T>
	class RflClassTypeRegistrarImpl : public RflClassRegistrar
	{
	public:

		RflClassTypeRegistrarImpl(RflClassId typeId, const std::string &typeName)
		: RflClassRegistrar(typeId, typeName)
		{
		}

		virtual RflClass *createClassInstance(RflFieldProbe *probe)
		{
		    return new T(probe); // FIXME: RAII!!!
		}
	};


	class RiotFunctionLibrary
	{
	public:

		friend class RflClassRegistrar;


		RiotFunctionLibrary(const std::string &name);

		inline std::string getName() const { return mName; }
		inline size_t getClassTypeCount() const { return mRegistrarMap.size(); }
		RflClassRegistrar &getClassRegistrarById(RflClassId id);


		inline static RiotFunctionLibrary &getSingleton() { return smSingleton; }


	private:

		std::string mName;
		std::map<RflClassId, std::reference_wrapper<RflClassRegistrar>> mRegistrarMap;

		static RiotFunctionLibrary smSingleton;
	};
}


#endif /* INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_ */

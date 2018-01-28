/*
 * RiotFunctionLibrary.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_
#define INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_

#include <string>
#include <map>

#include "RflClassType.h"

#define OD_REGISTER_RFL_CLASSTYPE(typeId, typeName, classTypeCppClass) \
	static od::RflClassTypeRegistrarImpl<classTypeCppClass> sOdRflRegistrar_ ## _ ## classTypeCppClass (typeId, typeName);

#define OD_REGISTER_RFL(rflName) \
	od::RiotFunctionLibrary od::RiotFunctionLibrary::smSingleton(rflName);

namespace od
{

	typedef uint16_t RflClassTypeId;

	class RiotObject;
	class RiotFunctionLibrary;

	class RflClassTypeRegistrar
	{
	public:

		RflClassTypeRegistrar(RflClassTypeId typeId, const std::string &typeName);
		virtual ~RflClassTypeRegistrar();

		virtual RflClassType &getClassType() = 0;

		inline RflClassTypeId getTypeId() const { return mTypeId; }
		inline std::string getTypeName() const { return mTypeName; }


	private:

		RflClassTypeId mTypeId;
		std::string mTypeName;

	};


	template <typename T>
	class RflClassTypeRegistrarImpl : public RflClassTypeRegistrar
	{
	public:

		RflClassTypeRegistrarImpl(RflClassTypeId typeId, const std::string &typeName)
		: RflClassTypeRegistrar(typeId, typeName)
		{
		}

		virtual RflClassType &getClassType()
		{
			return mClassType;
		}

	private:

		T mClassType;
	};


	class RiotFunctionLibrary
	{
	public:

		friend class RflClassTypeRegistrar;


		RiotFunctionLibrary(const std::string &name);

		inline std::string getName() const { return mName; }
		inline size_t getClassTypeCount() const { return mRegistrarMap.size(); }
		RflClassType &getClassTypeById(RflClassTypeId id);


		inline static RiotFunctionLibrary &getSingleton() { return smSingleton; }


	private:

		std::string mName;
		std::map<RflClassTypeId, RflClassTypeRegistrar*> mRegistrarMap;

		static RiotFunctionLibrary smSingleton;
	};
}


#endif /* INCLUDE_RFL_RIOTFUNCTIONLIBRARY_H_ */

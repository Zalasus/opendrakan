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
#include "Logger.h"

#define OD_REGISTER_RFL_CLASS(classId, className, classCppClass) \
	static odRfl::RflClassRegistrarImpl<classCppClass> sOdRflRegistrar_ ## _ ## classCppClass (classId, className);

#define OD_REGISTER_RFL(rflName) \
	odRfl::Rfl::RflNameSetter sRflNameSetter(rflName);

namespace odRfl
{

	typedef uint16_t RflClassId;

	class Rfl;

	class RflClassRegistrar
	{
	public:

		RflClassRegistrar(RflClassId classId, const std::string &className);
		virtual ~RflClassRegistrar();

		virtual std::unique_ptr<RflClass> createClassInstance() = 0;

		inline RflClassId getClassId() const { return mClassId; }
		inline std::string getClassName() const { return mClassName; }


	protected:

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

		virtual std::unique_ptr<RflClass> createClassInstance() override
		{
			Logger::debug() << "Creating instance of RFL class '" << mClassName << "' (" << std::hex << mClassId << std::dec << ")";

		    return std::unique_ptr<RflClass>(new T);
		}
	};


	class Rfl
	{
	public:

		class RflNameSetter // xD
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

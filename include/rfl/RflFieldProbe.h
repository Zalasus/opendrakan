/*
 * RflFieldProbe.h
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLCLASSBUILDER_H_
#define INCLUDE_RFL_RFLCLASSBUILDER_H_

#include <string>
#include <vector>

#include "RflField.h"

namespace odRfl
{

    /**
     * Common interface for classes that can be used to probe an RflClass for it's fields,
     * possibly filling them while they do (see RflClassBuilder).
     */
	class RflFieldProbe
	{
	public:

	    virtual ~RflFieldProbe() = default;

	    /// Starts a new category. Categories may appear more than once. Fields will be added to the previous definition
	    virtual void beginCategory(const char *categoryName) = 0;

	    /// Registers a field in this probe. Should only be used by RflClass children.
		virtual void registerField(RflField &field, const char *fieldName) = 0; // TODO: Could use std::string_view once we switch to C++17

		RflFieldProbe &operator()(RflField &field, const char *fieldName) { registerField(field, fieldName); return *this; }
		RflFieldProbe &operator()(const char *categoryName) { beginCategory(categoryName); return *this; }
	};

	class RflClassBuilder : public RflFieldProbe
	{
	public:

	    RflClassBuilder();

		void readFieldRecord(od::DataReader &dr, bool isObjectRecord);

		/// Resets internal index counter so this builder can be used to build another class.
		void resetIndexCounter();

		virtual void beginCategory(const char *categoryName) override;
		virtual void registerField(RflField &field, const char *fieldName) override;


	private:

		struct FieldEntry
		{
			RflField::RflFieldType fieldType;
			bool isArray;
			uint16_t index;
			size_t dataOffset; // offset of entry in mFieldData
	        std::string fieldName;
		};

	    std::vector<FieldEntry> mFieldEntries;
	    std::vector<char> mFieldData;
	    size_t mRegistrationIndex;
	};

	class RflObjectBuilder : public RflFieldProbe
	{
	};

}

#endif /* INCLUDE_RFL_RFLCLASSBUILDER_H_ */

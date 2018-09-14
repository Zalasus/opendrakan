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

namespace od
{
    class DataReader;
    class AssetProvider;
}

namespace odRfl
{

    class RflField;
    class RflAssetRef;

    /**
     * Common interface for classes that can be used to probe an RflClass for it's fields,
     * possibly filling them while they do (see RflClassBuilder).
     */
	class RflFieldProbe
	{
	public:

	    virtual ~RflFieldProbe() = default;

	    /// Starts a new category. Categories may appear more than once. Fields will be added to the previous definition
	    virtual void beginCategory(const char *categoryName);

	    /// Registers a field in this probe. Should only be used by RflClass children.
		virtual void registerField(RflField &field, const char *fieldName); // TODO: Could use std::string_view once we switch to C++17

		/// Registers an asset ref field in this probe. Default behaviour is to pass field to generic method.
		virtual void registerField(RflAssetRef &field, const char *fieldName);

		RflFieldProbe &operator()(const char *categoryName) { beginCategory(categoryName); return *this; }

		template <typename T>
		inline RflFieldProbe &operator()(T &field, const char *fieldName) { registerField(field, fieldName); return *this; }

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
			uint32_t fieldType;
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

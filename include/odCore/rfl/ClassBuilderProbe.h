/*
 * ClassBuilderProbe.h
 *
 *  Created on: Sep 17, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_CLASSBUILDERPROBE_H_
#define INCLUDE_ODCORE_RFL_CLASSBUILDERPROBE_H_

#include <odCore/rfl/FieldProbe.h>

namespace odRfl
{

    class ClassBuilderProbe : public FieldProbe
    {
    public:

        ClassBuilderProbe();

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

}


#endif /* INCLUDE_ODCORE_RFL_CLASSBUILDERPROBE_H_ */

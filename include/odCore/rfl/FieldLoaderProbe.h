
#ifndef INCLUDE_ODCORE_RFL_FIELDLOADERPROBE_H_
#define INCLUDE_ODCORE_RFL_FIELDLOADERPROBE_H_

#include <vector>

#include <odCore/rfl/FieldProbe.h>

namespace od
{
    class DataReader;
}

namespace odRfl
{

    /**
     * @brief A FieldProbe that can read and permanently store the data from a field record, and use it to fill FieldBundles.
     */
    class FieldLoaderProbe final : public FieldProbe
    {
    public:

        enum class RecordFormat
        {
            OBJECT,
            CLASS
        };

        FieldLoaderProbe();

        FieldLoaderProbe(FieldLoaderProbe &&p) = default;

        /**
         * Field records in object and class data store basically the same data, but use a slightly different format.
         * Thus, you have to specify the format that you want to be read.
         */
        void loadFromRecord(od::DataReader &dr, RecordFormat format);

        FieldLoaderProbe &operator=(FieldLoaderProbe &&p) = default;

        /// Resets internal index counter so this builder can be used to build another class.
        void reset();

        virtual void beginCategory(const char *categoryName) override;
        virtual void registerField(Field &field, const char *fieldName) override;


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

#endif

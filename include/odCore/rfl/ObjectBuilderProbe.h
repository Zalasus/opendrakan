/*
 * ObjectBuilderProbe.h
 *
 *  Created on: 19 Oct 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_OBJECTBUILDERPROBE_H_
#define INCLUDE_ODCORE_RFL_OBJECTBUILDERPROBE_H_

#include <vector>

#include <odCore/rfl/FieldProbe.h>

namespace od
{
    class DataReader;
}

namespace odRfl
{

    class ObjectBuilderProbe : public FieldProbe
    {
    public:

        ObjectBuilderProbe();

        void readFieldRecord(od::DataReader &dr);

        virtual void beginCategory(const char *categoryName) override;
        virtual void registerField(Field &field, const char *fieldName) override;


    private:

        struct FieldRegistration
        {
            FieldRegistration(Field &f, const char *name) : field(f), fieldName(name) {}
            Field &field;
            std::string fieldName;
        };

        std::vector<FieldRegistration> mFieldRegistrations;
    };

}

#endif /* INCLUDE_ODCORE_RFL_OBJECTBUILDERPROBE_H_ */

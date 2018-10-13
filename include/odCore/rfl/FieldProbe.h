/*
 * FieldProbe.h
 *
 *  Created on: Sep 17, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RFL_FIELDPROBE_H_
#define INCLUDE_ODCORE_RFL_FIELDPROBE_H_

#include <string>
#include <vector>

namespace odRfl
{
    class RflField;
    class RflAssetRef;

    /**
     * Common interface for classes that can be used to probe an RflClass for it's fields,
     * possibly filling them while they do (see RflClassBuilder).
     */
    class FieldProbe
    {
    public:

        virtual ~FieldProbe() = default;

        /// Starts a new category. Categories may appear more than once. Fields will be added to the previous definition
        virtual void beginCategory(const char *categoryName);

        /// Registers a field in this probe. Should only be used by RflClass children.
        virtual void registerField(RflField &field, const char *fieldName); // TODO: Could use std::string_view once we switch to C++17

        /// Registers an asset ref field in this probe. Default behaviour is to pass field to generic method.
        virtual void registerField(RflAssetRef &field, const char *fieldName);

        FieldProbe &operator()(const char *categoryName) { beginCategory(categoryName); return *this; }

        template <typename T>
        inline FieldProbe &operator()(T &field, const char *fieldName) { registerField(field, fieldName); return *this; }

    };

}


#endif /* INCLUDE_ODCORE_RFL_FIELDPROBE_H_ */

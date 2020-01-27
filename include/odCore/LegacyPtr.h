/**
 * @file LegacyPtr.h
 * @date Jan 26, 2020
 * @author zal
 */

#ifndef INCLUDE_ODCORE_LEGACYPTR_H_
#define INCLUDE_ODCORE_LEGACYPTR_H_

#include <memory>

namespace od
{

    template <typename T, void(*_Deleter)(T*)>
    class LegacyDeleter
    {
    public:

        void operator()(T *ptr) const
        {
            _Deleter(ptr);
        }

    };


    template <typename T, void(*_Deleter)(T*)>
    using LegacyUniquePtr = std::unique_ptr<T, LegacyDeleter<T, _Deleter>>;


}


#endif /* INCLUDE_ODCORE_LEGACYPTR_H_ */

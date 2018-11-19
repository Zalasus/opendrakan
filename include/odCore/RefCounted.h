/*
 * RefCounted.h
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_REFCOUNTED_H_
#define INCLUDE_ODCORE_REFCOUNTED_H_

#include <stdlib.h>
#include <type_traits>

namespace od
{

    class RefCounted;

    class ReferenceObserver
    {
    public:

        virtual ~ReferenceObserver() = default;

        virtual void onReferencedAboutToBeDestroyed(RefCounted *r) = 0;
    };


    class RefCounted
    {
    public:

        RefCounted();

        size_t referenceCreated();
        size_t referenceDestroyed();
        size_t referenceReleased();
        size_t getReferenceCount();
        void setReferenceObserver(ReferenceObserver *observer);


    private:

        size_t mRefCount;
        ReferenceObserver *mObserver;
    };


    template <typename T>
    class RefPtr
    {
    public:

        static_assert(std::is_base_of<RefCounted, T>::value, "T in od::RefPtr<T> must have od::RefCounted as a base!");

        RefPtr()
        : mPtr(nullptr)
        {
        }

        RefPtr(T *ptr)
        : mPtr(ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }
        }

        RefPtr(const RefPtr<T> &refPtr)
        : mPtr(refPtr.mPtr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }
        }

        ~RefPtr()
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceDestroyed();
                if(mPtr->getReferenceCount() <= 0)
                {
                    delete mPtr;
                }
            }
        }

        RefPtr<T> &operator=(T *ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceDestroyed();
                if(mPtr->getReferenceCount() <= 0)
                {
                    delete mPtr;
                }
            }

            mPtr = ptr;

            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }

            return *this;
        }

        RefPtr<T> &operator=(const RefPtr<T> &refPtr)
        {
            return this->operator=(refPtr.mPtr);
        }

        operator T*() const
        {
            return mPtr;
        }

        T *operator->() const
        {
            return mPtr;
        }

        T *release()
        {
            T *ptr = mPtr;
            mPtr = nullptr;

            if(ptr != nullptr)
            {
                ptr->referenceDestroyed();
            }

            return ptr;
        }

        T *get()
        {
            return mPtr;
        }


    private:

        T *mPtr;

    };

}


#endif /* INCLUDE_ODCORE_REFCOUNTED_H_ */

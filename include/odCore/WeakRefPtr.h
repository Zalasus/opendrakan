/*
 * WeakRefPtr.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_WEAKREFPTR_H_
#define INCLUDE_ODCORE_WEAKREFPTR_H_

#include <odCore/RefCounted.h>

namespace od
{

    template <typename T>
    class WeakRefPtr : public ReferenceObserver
    {
    public:

        WeakRefPtr()
        : mPtr(nullptr)
        {
        }

        WeakRefPtr(T *ptr)
        : mPtr(ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->addReferenceObserver(this);
            }
        }

        WeakRefPtr(const WeakRefPtr<T> &refPtr)
        : mPtr(refPtr.mPtr)
        {
            if(mPtr != nullptr)
            {
                mPtr->addReferenceObserver(this);
            }
        }

        ~WeakRefPtr()
        {
            if(mPtr != nullptr)
            {
                mPtr->removeReferenceObserver(this);
            }
        }

        WeakRefPtr<T> &operator=(T *ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->removeReferenceObserver(this);
            }

            mPtr = ptr;

            if(mPtr != nullptr)
            {
                mPtr->addReferenceObserver(this);
            }

            return *this;
        }

        WeakRefPtr<T> &operator=(const WeakRefPtr<T> &weakPtr)
        {
            return this->operator=(weakPtr.mPtr);
        }

        operator T*() const
        {
            return mPtr;
        }

        T *operator->() const
        {
            return mPtr;
        }

        T *get()
        {
            return mPtr;
        }


    protected:

        virtual void onReferencedAboutToBeDestroyed(RefCounted *r) override
        {
            if(static_cast<T*>(r) != mPtr)
            {
                return;
            }

            mPtr = nullptr;
        }


    private:

        T *mPtr;

    };

}

#endif /* INCLUDE_ODCORE_WEAKREFPTR_H_ */

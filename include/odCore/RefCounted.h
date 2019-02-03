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
#include <utility>

namespace od
{

    class RefCounted;

    class ReferenceObserver
    {
    public:

        friend class RefCounted;

        virtual ~ReferenceObserver() = default;


    protected:

        virtual void onReferencedAboutToBeDestroyed(RefCounted *r) = 0;

    };


    class RefCounted
    {
    public:



        RefCounted();
        virtual ~RefCounted();

        size_t referenceCreated();
        size_t referenceDestroyed();
        size_t referenceReleased();
        size_t getReferenceCount();

        void addReferenceObserver(ReferenceObserver *observer);
        void removeReferenceObserver(ReferenceObserver *observer);


    protected:

        static const size_t MaxObservers = 4;


    private:

        void _notifyAllObservers();

        size_t mRefCount;
        size_t mObserverCount;
        ReferenceObserver *mObservers[MaxObservers];
    };


    template <typename T>
    class RefPtr
    {
    public:

        //static_assert(std::is_base_of<RefCounted, T>::value, "T in od::RefPtr<T> must have od::RefCounted as a base!");

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

        RefPtr(RefPtr<T> &&refPtr)
        : mPtr(refPtr.mPtr)
        {
            refPtr.mPtr = nullptr;
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
                ptr->referenceReleased();
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


    template <typename T, typename... Args>
    RefPtr<T> make_refd(Args&&... args)
    {
        return od::RefPtr<T>(new T(std::forward<Args>(args)...));
    }

}


#endif /* INCLUDE_ODCORE_REFCOUNTED_H_ */

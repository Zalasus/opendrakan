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
#include <atomic>

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


    struct RefControlBlock
    {
    public:

        friend class RefCounted;

        size_t weakRefCount;
        std::atomic_bool refExpired;


    private:

        RefControlBlock()
        : weakRefCount(0)
        , refExpired(false)
        {
        }

    };


    class RefCounted
    {
    public:

        RefCounted();
        virtual ~RefCounted();

        inline size_t getReferenceCount() const { return mRefCount; }
        inline RefControlBlock *getRefControlBlock() { return mRefControlBlock; }

        /**
         * @brief Decrements the reference count atomically.
         * @return The value the reference count had *before* incrementing
         */
        size_t referenceCreated();

        /**
         * @brief Decrements the reference count atomically.
         * @return The value the reference count had *before* incrementing
         */
        size_t referenceDestroyed();

        /**
         * @brief Notifies all observers that the object is about to be destroyed.
         */
        void notifyAllObservers();

        void addReferenceObserver(ReferenceObserver *observer);
        void removeReferenceObserver(ReferenceObserver *observer);

        RefControlBlock *getOrCreateRefControlBlock();


    protected:

        static const size_t MaxObservers = 4;


    private:

        void _notifyAllObservers();

        std::atomic_size_t mRefCount;
        size_t mObserverCount;
        ReferenceObserver *mObservers[MaxObservers];
        RefControlBlock *mRefControlBlock;
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
            _unref();
        }

        RefPtr<T> &operator=(T *ptr)
        {
            _unref();

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

        void _unref()
        {
            if(mPtr != nullptr)
            {
                size_t oldRefCount = mPtr->referenceDestroyed();
                if(oldRefCount <= 1)
                {
                    mPtr->notifyAllObservers();
                    delete mPtr;
                    mPtr = nullptr;
                }
            }
        }

        T *mPtr;

    };


    template <typename T, typename... Args>
    RefPtr<T> make_refd(Args&&... args)
    {
        return od::RefPtr<T>(new T(std::forward<Args>(args)...));
    }

}


#endif /* INCLUDE_ODCORE_REFCOUNTED_H_ */

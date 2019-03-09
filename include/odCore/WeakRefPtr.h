/*
 * WeakRefPtr.h
 *
 *  Created on: 27 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_WEAKREFPTR_H_
#define INCLUDE_ODCORE_WEAKREFPTR_H_

#include <cassert>

#include <odCore/RefCounted.h>

namespace od
{

    template <typename T>
    class WeakRefPtr
    {
    public:

        WeakRefPtr()
        : mPtr(nullptr)
        , mControlBlock(nullptr)
        {
        }

        explicit WeakRefPtr(T *p)
        : mPtr(p)
        , mControlBlock(nullptr)
        {
            if(mPtr != nullptr)
            {
                mControlBlock = mPtr->getOrCreateRefControlBlock();
                mControlBlock->weakRefCount++;
            }
        }

        WeakRefPtr(const WeakRefPtr<T> &p)
        : mPtr(p.mPtr)
        , mControlBlock(p.mControlBlock)
        {
            if(mControlBlock != nullptr)
            {
                mControlBlock->weakRefCount++;
            }
        }

        ~WeakRefPtr()
        {
            _removeRef();
        }

        WeakRefPtr<T> &operator=(T *p)
        {
            _removeRef();

            mPtr = p;
            if(mPtr != nullptr)
            {
                mControlBlock = mPtr->getOrCreateRefControlBlock();
                mControlBlock->weakRefCount++;
            }

            return *this;
        }

        WeakRefPtr<T> & operator=(const WeakRefPtr<T> &p)
        {
            _removeRef();

            mPtr = p.mPtr;
            mControlBlock = p.mControlBlock;

            if(mControlBlock != nullptr)
            {
                mControlBlock->weakRefCount++;

            }else
            {
                assert(mPtr == nullptr);
            }

            return *this;
        }

        bool isNull()
        {
            if(mControlBlock != nullptr && !mControlBlock->refExpired)
            {
                assert(mPtr != nullptr);
                return true;

            }else
            {
                return false;
            }
        }

        bool isNonNull()
        {
            return !isNull();
        }

        RefPtr<T> aquire()
        {
            if(mControlBlock != nullptr && !mControlBlock->refExpired)
            {
                assert(mPtr != nullptr);

                return RefPtr<T>(mPtr);

            }else
            {
                // if object does no longer exist, and we are the only weak reference, we might as well
                //  deallocate the control block since no one will be using it, anyway
                if(mControlBlock != nullptr && mControlBlock->weakRefCount == 1)
                {
                    delete mControlBlock;
                    mControlBlock = nullptr;
                    mPtr = nullptr;
                }

                return RefPtr<T>(nullptr);
            }
        }


    private:

        void _removeRef()
        {
            if(mControlBlock != nullptr)
            {
                mControlBlock->weakRefCount--;
                if(mControlBlock->weakRefCount == 0 && mControlBlock->refExpired)
                {
                    delete mControlBlock;
                    mControlBlock = nullptr;
                }
            }

            mPtr = nullptr;
        }

        T *mPtr;
        RefControlBlock *mControlBlock;

    };


    template <typename T>
    class WeakObserverRefPtr : public ReferenceObserver
    {
    public:

        WeakObserverRefPtr()
        : mPtr(nullptr)
        {
        }

        explicit WeakObserverRefPtr(T *ptr)
        : mPtr(ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->addReferenceObserver(this);
            }
        }

        WeakObserverRefPtr(const WeakObserverRefPtr<T> &refPtr)
        : mPtr(refPtr.mPtr)
        {
            if(mPtr != nullptr)
            {
                mPtr->addReferenceObserver(this);
            }
        }

        ~WeakObserverRefPtr()
        {
            if(mPtr != nullptr)
            {
                mPtr->removeReferenceObserver(this);
            }
        }

        WeakObserverRefPtr<T> &operator=(T *ptr)
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

        WeakObserverRefPtr<T> &operator=(const WeakObserverRefPtr<T> &weakPtr)
        {
            return this->operator=(weakPtr.mPtr);
        }

        bool isNull()
        {
            return mPtr == nullptr;
        }

        bool isNonNull()
        {
            return !isNull();
        }

        RefPtr<T> aquire()
        {
            return RefPtr<T>(mPtr);
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

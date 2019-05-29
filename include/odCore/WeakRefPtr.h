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

        bool operator==(T* p) const
        {
            if(p == nullptr)
            {
                return isNull();

            }else
            {
                if(mControlBlock == nullptr || mControlBlock->refExpired)
                {
                    return false;

                }else
                {
                    return mPtr == p;
                }
            }
        }

        bool isNull() const
        {
            return !_ptrValidAndNonNull();
        }

        bool isNonNull() const
        {
            return !isNull();
        }

        RefPtr<T> aquire() const
        {
            if(_ptrValidAndNonNull())
            {
                return RefPtr<T>(mPtr);

            }else
            {
                // if object does no longer exist, there is no point in keeping a ref to the control block here
                _removeRef();

                return RefPtr<T>(nullptr);
            }
        }


    private:

        bool _ptrValidAndNonNull() const
        {
            if(mControlBlock != nullptr && !(mControlBlock->refExpired))
            {
                assert(mPtr != nullptr);
                return true;

            }else
            {
                return false;
            }
        }

        void _removeRef() const
        {
            if(mControlBlock != nullptr)
            {
                assert(mControlBlock->weakRefCount > 0);
                mControlBlock->weakRefCount--;
                if(mControlBlock->weakRefCount == 0 && mControlBlock->refExpired)
                {
                    delete mControlBlock;
                }
            }

            mControlBlock = nullptr;
            mPtr = nullptr;
        }

        mutable T *mPtr;
        mutable RefControlBlock *mControlBlock;

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

        bool isNull() const
        {
            return mPtr == nullptr;
        }

        bool isNonNull() const
        {
            return !isNull();
        }

        RefPtr<T> aquire() const
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

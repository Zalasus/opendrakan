/*
 * RefCounted.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */


#include <odCore/RefCounted.h>

#include <odCore/Logger.h>
#include <odCore/Exception.h>

namespace od
{

    RefCounted::RefCounted()
    : mRefCount(0)
    , mObserverCount(0)
    , mRefControlBlock(nullptr)
    {
    }

    RefCounted::~RefCounted()
    {
        if(mRefCount != 0)
        {
            Logger::warn() << "RefCounted object was destroyed when references to it still existed";
        }

        if(mRefControlBlock != nullptr)
        {
            mRefControlBlock->refExpired = true;

            if(mRefControlBlock->weakRefCount == 0)
            {
                delete mRefControlBlock;
                mRefControlBlock = nullptr;
            }
        }
    }

    size_t RefCounted::referenceCreated()
    {
        return ++mRefCount;
    }

    size_t RefCounted::referenceDestroyed()
    {
        if(mRefCount > 0)
        {
            --mRefCount;

            if(mRefCount == 0)
            {
                _notifyAllObservers();
            }

        }else
        {
            throw Exception("Destroyed reference to RefCounted object when it already had 0 references");
        }

        return mRefCount;
    }

    size_t RefCounted::referenceReleased()
    {
        if(mRefCount > 0)
        {
            --mRefCount;

        }else
        {
            throw Exception("Released reference to RefCounted object when it already had 0 references");
        }

        return mRefCount;
    }

    size_t RefCounted::getReferenceCount()
    {
        return mRefCount;
    }

    void RefCounted::addReferenceObserver(ReferenceObserver *observer)
    {
        if(mObserverCount >= MaxObservers)
        {
            throw UnsupportedException("Tried to add more than max observers to RefCounted object");
        }

        if(observer == nullptr)
        {
            return;
        }

        mObservers[mObserverCount] = observer;
        ++mObserverCount;
    }

    void RefCounted::removeReferenceObserver(ReferenceObserver *observer)
    {
        if(observer == nullptr)
        {
           return;
        }

        // TODO: this may not be the most efficient way to handle multiple observers. maybe daisy-chain them instead?

        bool found = false;
        for(size_t i = 0; i < mObserverCount; ++i)
        {
            if(!found && mObservers[i] == observer)
            {
                found = true;

            }else if(found)
            {
                mObservers[i-1] = mObservers[i];
            }
        }

        --mObserverCount;
    }

    RefControlBlock *RefCounted::getOrCreateRefControlBlock()
    {
        if(mRefControlBlock == nullptr)
        {
            mRefControlBlock = new RefControlBlock();
        }

        return mRefControlBlock;
    }

    void RefCounted::_notifyAllObservers()
    {
        for(size_t i = 0; i < mObserverCount; ++i)
        {
            if(mObservers[i] != nullptr)
            {
                mObservers[i]->onReferencedAboutToBeDestroyed(this);
            }
        }
    }

}


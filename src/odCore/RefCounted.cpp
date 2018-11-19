/*
 * RefCounted.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */


#include <odCore/RefCounted.h>

#include <odCore/Logger.h>

namespace od
{

    RefCounted::RefCounted()
    : mRefCount(0)
    , mObserver(nullptr)
    {
    }

    RefCounted::~RefCounted()
    {
        if(mRefCount != 0)
        {
            Logger::warn() << "RefCounted object was destroyed when references to it still existed";
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

            if(mRefCount == 0 && mObserver != nullptr)
            {
                mObserver->onReferencedAboutToBeDestroyed(this);
            }
        }

        return mRefCount;
    }

    size_t RefCounted::referenceReleased()
    {
        if(mRefCount > 0)
        {
            --mRefCount;
        }

        return mRefCount;
    }

    size_t RefCounted::getReferenceCount()
    {
        return mRefCount;
    }

    void RefCounted::setReferenceObserver(ReferenceObserver *observer)
    {
        if(mObserver != nullptr)
        {
            Logger::warn() << "Overwriting ReferenceObserver in reference counted object. This may have unexpected results";
        }

        mObserver = observer;
    }

}


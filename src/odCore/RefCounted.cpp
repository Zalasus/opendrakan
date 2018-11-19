/*
 * RefCounted.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */


#include <odCore/RefCounted.h>

namespace od
{

    RefCounted::RefCounted()
    : mRefCount(0)
    , mObserver(nullptr)
    {
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

}


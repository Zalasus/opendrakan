/*
 * UpdateCallback.cpp
 *
 *  Created on: Aug 31, 2018
 *      Author: zal
 */

#include "UpdateCallback.h"

namespace od
{

    UpdateCallback::UpdateCallback()
    : mRelTime(0)
    , mSimTime(0)
    , mFirstUpdate(true)
    , mLastSimTime(0)
    {
    }

    void UpdateCallback::tick(const osg::FrameStamp *fs)
    {
        if(fs != nullptr)
        {
            if(mFirstUpdate)
            {
                mLastSimTime = fs->getSimulationTime();
                mFirstUpdate = false;
            }

            mSimTime = fs->getSimulationTime();
            mRelTime = mSimTime - mLastSimTime;
            mLastSimTime = mSimTime;
        }
    }

}




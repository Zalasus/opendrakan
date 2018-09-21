/*
 * UpdateCallback.h
 *
 *  Created on: Aug 31, 2018
 *      Author: zal
 */

#ifndef INCLUDE_UPDATECALLBACK_H_
#define INCLUDE_UPDATECALLBACK_H_

#include <osg/NodeCallback>
#include <osg/FrameStamp>

namespace od
{

    // convenience callback providing glitch-free absolute- and relative timing
    class UpdateCallback : public osg::NodeCallback
    {
    protected:

        UpdateCallback();

        void tick(const osg::FrameStamp *fs);

        double mRelTime;
        double mSimTime;

    private:

        bool mFirstUpdate;
        double mLastSimTime;

    };

}

#endif /* INCLUDE_UPDATECALLBACK_H_ */

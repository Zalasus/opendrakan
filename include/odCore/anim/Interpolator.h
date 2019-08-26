/*
 * Interpolator.h
 *
 *  Created on: 28 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_ANIM_INTERPOLATOR_H_
#define INCLUDE_ODCORE_ANIM_INTERPOLATOR_H_

#include <type_traits>
#include <cmath>

#include <odCore/Logger.h>

namespace odAnim
{

    template <typename T>
    struct LinearInterpolator
    {
        inline T operator()(const T &start, const T &end, float delta)
        {
            return start*(1 - delta) + end*delta;
        }
    };

    template <typename T>
    struct SineInterpolator
    {
        inline T operator()(const T &start, const T &end, float delta)
        {
            LinearInterpolator<T> lerp;

            delta = std::sin((delta-0.5)*3.141592654)*0.5 + 0.5;
            return lerp(start, end, delta);
        }
    };

    template <typename T, typename _Interpolator = LinearInterpolator<T>>
    class Interpolated
    {
    public:

        typedef float _TimeType;

        Interpolated(const T &value)
        : mValue(value)
        , mStart(value)
        , mTarget(value)
        , mActive(false)
        , mLastTime(0.0)
        , mDuration(0.0)
        {
        }

        /// @brief Immediately sets the value to \c v, ending any running interpolation.
        void set(const T &v)
        {
            mValue = v;
            mActive = false;
        }

        /// @brief Moves the value to \c target over an interval of \c duration time units.
        void move(const T &target, _TimeType duration)
        {
            mStart = mValue;
            mTarget = target;

            mDuration = duration;
            mLastTime = 0.0;
            mActive = true;
        }

        /// @brief Sets the value to \c start, then moves the value to \c target over an interval of \c duration time units.
        void move(const T &start, const T &target, _TimeType duration)
        {
            set(start);
            move(target, duration);
        }

        /// @brief Updates the value according to the interpolation function.
        /// @param  relTime  The time passed since the last call to update()
        /// @return true if the interpolated value has changed, false if not
        bool update(_TimeType relTime)
        {
            if(!mActive)
            {
                return false;
            }

            _TimeType currentTime = mLastTime + relTime;
            _TimeType timeDelta = currentTime/mDuration;
            T newValue = mValue;
            if(timeDelta > 1.0)
            {
                timeDelta = 1.0;
                newValue = mTarget;
                mActive = false;

            }else
            {
                // need to interpolate
                newValue = mInterpolator(mStart, mTarget, timeDelta);
            }

            bool changed = (newValue != mValue);
            mValue = newValue;
            mLastTime = currentTime;

            return changed;
        }

        T get() const
        {
            return mValue;
        }

        operator T () const
        {
            return mValue;
        }


    private:

        T mValue;
        T mStart;
        T mTarget;
        bool mActive;
        _TimeType mLastTime;
        _TimeType mDuration;
        _Interpolator mInterpolator;
    };

}

#endif /* INCLUDE_ODCORE_ANIM_INTERPOLATOR_H_ */

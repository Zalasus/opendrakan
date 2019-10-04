/*
 * @file State.h
 * @date 14 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

namespace odState
{

    template <typename T>
    class State
    {
    public:

        void force(const T &v)
        {
            mValue = v;
        }

        void submitChange(const T &v)
        {
            mTarget = v;
            mUncommittedChanges = true;
        }

        bool hasChanged()
        {
            return mUncommittedChanges;
        }

        T commit()
        {
            if(mUncommittedChanges)
            {
                auto prevValue = mValue;
                mValue = mTarget;
                mUncommittedChanges = false;
                return prevValue;
            }

            return mValue;
        }

        const T &get() const
        {
            return mValue;
        }


    private:

        T mValue;

        T mTarget;
        bool mUncommittedChanges;

    };

}

#endif /* INCLUDE_ODCORE_STATE_STATE_H_ */

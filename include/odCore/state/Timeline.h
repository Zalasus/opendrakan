/*
 * @file Timeline.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_TIMELINE_H_
#define INCLUDE_ODCORE_STATE_TIMELINE_H_

#include <deque>
#include <type_traits>

namespace odState
{

    using TickNumber = std::int64_t;

    static_assert(std::is_signed<TickNumber>::value, "TickNumber must be of a signed type");

    /**
     * @brief A kind of vector that ties together blocks of elements based on a time value.
     *
     * This is basically just intended to manage events and interpolation. However, since i'm not sure how i'll
     * implement polymorphism over events, I decided to make this a template.
     */
    template <typename T>
    class Timeline
    {
    public:

        using Iterator = typename std::deque<T>::const_iterator;

        Timeline(TickNumber initialTick)
        : mFirstTick(initialTick)
        {
            // by our own internal contract, the offset vector always has to
            //  contain at least an entry for mFirstTickNumber
            mTickOffsets.emplace_back(0);
        }

        TickNumber getFirstTick() const
        {
            return mFirstTick;
        }

        TickNumber getLastTick() const
        {
            return mFirstTick + mTickOffsets.size();
        }

        std::size_t getTickCount() const
        {
            return mTickOffsets.size();
        }

        std::size_t getEventCountInTick(TickNumber tick) const
        {
            // do we have the requested tick in our timeline? if not, zero is the correct answer
            if((tick < mFirstTick) || (tick >= mFirstTick + mTickOffsets.size()))
            {
                return 0;
            }

            std::size_t indexInOffsetArray = tick - mFirstTick;

            // generally, the size of any tick is given by the difference between it's offset and the
            //  next tick's offset. the tick after the last tick, however, has no entry in the offset vector
            //  and thus we have to use the size of the element vector as an offset. simple as that
            if(indexInOffsetArray == mTickOffsets.size() - 1)
            {
                 return mElements.size() - mTickOffsets[indexInOffsetArray];

            }else
            {
                return mTickOffsets[indexInOffsetArray + 1] - mTickOffsets[indexInOffsetArray];
            }
        }

        void push(const T &event)
        {
            mElements.push_back(event);
        }

        void push(T &&event)
        {
            mElements.emplace_back(event);
        }

        void nextTick()
        {
            // edge case: if we only store one tick, and that one is empty, it
            //  is more efficient to just move mFirstTick forward by one,
            //  keeping all other state the same.
            if(mTickOffsets.size() == 1 && mTickOffsets[0] == 0)
            {
                ++mFirstTick;
                return;
            }

            mTickOffsets.emplace_back(mElements.size());
        }

        void dropFirstTick()
        {
            ++mFirstTick;

            if(mTickOffsets.size() > 1)
            {
                mTickOffsets.pop_front();
                std::size_t firstTickElements = mTickOffsets.front();
                mElements.erase(mElements.begin(), mElements.begin() + firstTickElements);
                for(auto &offset : mTickOffsets)
                {
                    offset -= firstTickElements;
                }

            }else
            {
                mTickOffsets[0] = 0;
                mElements.clear();
            }
        }

        Iterator getTickframeBegin(TickNumber tick) const
        {
            // do we have the requested tick in our timeline?
            if((tick < mFirstTick) || (tick >= mFirstTick + mTickOffsets.size()))
            {
                return mElements.end();
            }

            std::size_t offset = mTickOffsets[tick - mFirstTick];

            return mElements.begin() + offset;
        }

        Iterator getTickframeEnd(TickNumber tick) const
        {
            if((tick < mFirstTick) || (tick >= mFirstTick + mTickOffsets.size() - 1))
            {
                return mElements.end();
            }

            std::size_t offset = mTickOffsets[tick - mFirstTick + 1];

            return mElements.begin() + offset;
        }


        class Tickframe
        {
        public:

            Tickframe(Iterator begin, Iterator end)
            : mBegin(begin)
            , mEnd(end)
            {
            }

            inline Iterator begin() const { return mBegin; }
            inline Iterator end() const { return mEnd; }

        private:

            Iterator mBegin;
            Iterator mEnd;

        };

        Tickframe getTickframe(TickNumber tick)
        {
            return Tickframe(getTickframeBegin(tick), getTickframeEnd(tick));
        }


    private:


        using TickMapping = std::pair<TickNumber, std::size_t>;

        TickNumber mFirstTick;
        // index t contains the absolute offset the first element in tick (t+mFirstTick) has in mElements
        std::deque<std::size_t> mTickOffsets;
        std::deque<T> mElements;

    };

}

#endif

/*
 * SimpleArray.h
 *
 *  Created on: 6 Feb 2016
 *      Author: zalasus
 */

#ifndef INCLUDE_SIMPLEARRAY_H_
#define INCLUDE_SIMPLEARRAY_H_

#include <stdint.h>
#include <string>
#include <stdexcept>

namespace od
{

    template <typename T>
    class SimpleArray
    {
    public:

        SimpleArray();
        SimpleArray(size_t size);
        SimpleArray(const SimpleArray<T> &s);
        ~SimpleArray();

        bool allocate(uint32_t size);
        uint32_t size() const;
        T *ptr() const;

        T &operator [](uint32_t i);
        const T &operator [](uint32_t i) const;
        operator T*() const;
        operator const T*() const;


    private:

        T *mData;
        uint32_t mSize;

    };

    template <typename T>
    SimpleArray<T>::SimpleArray()
    : mData(nullptr),
      mSize(0)
    {
    }

    template <typename T>
    SimpleArray<T>::SimpleArray(size_t size)
    : mData(nullptr),
      mSize(size)
    {
        allocate(size);
    }

    template <typename T>
    SimpleArray<T>::SimpleArray(const SimpleArray<T> &s)
    : mData(nullptr),
      mSize(0)
    {
        if(s.size())
        {
            allocate(s.size());

            for(uint32_t i = 0; i < size(); ++i)
            {
                mData[i] = s[i];
            }
        }
    }

    template <typename T>
    SimpleArray<T>::~SimpleArray()
    {
        if(mData != nullptr)
        {
            delete[] mData;
            mData = nullptr;
            mSize = 0;
        }
    }

    template <typename T>
    bool SimpleArray<T>::allocate(uint32_t size)
    {
        if(mData != nullptr)
        {
            return false;
        }

        mSize = size;

        if(mSize > 0)
        {
            mData = new T[mSize];
        }

        return true;
    }

    template <typename T>
    uint32_t SimpleArray<T>::size() const
    {
        return mSize;
    }

    template <typename T>
    T *SimpleArray<T>::ptr() const
    {
        return mData;
    }

    template <typename T>
    T &SimpleArray<T>::operator [](uint32_t i)
    {
        if(i >= mSize)
        {
            throw std::runtime_error("Requested index  out of bounds");
        }

        return mData[i];
    }

    template <typename T>
    const T &SimpleArray<T>::operator [](uint32_t i) const
    {
        if(i >= mSize)
        {
            throw std::runtime_error("Requested index out of bounds");
        }

        return mData[i];
    }

    template <typename T>
    SimpleArray<T>::operator T*() const
    {
        return mData;
    }

    template <typename T>
    SimpleArray<T>::operator const T*() const
    {
        return mData;
    }

}


#endif /* INCLUDE_SIMPLEARRAY_H_ */

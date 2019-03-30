/*
 * Downcast.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_DOWNCAST_H_
#define INCLUDE_ODCORE_DOWNCAST_H_

#include <odCore/Exception.h>

namespace od
{

    /**
     * An downcast which must succeed, and will throw otherwise.
     *
     * In contrast to od::confident_downcast(), this will always perform a dynamic_cast.
     */
    template <typename _To, typename _From>
    _To *downcast(_From *f)
    {
       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad downcast");
       }

       return t;
    }

    /**
     * A style of downcast for when the caller is confident that it will always succeed,
     * such as when casting to implementations of interfaces of which only one should
     * exist in the application (e.g. renderer, physics system...).
     *
     * In contrast to od::downcast(), this might actually do a static_cast for efficiency,
     * while using a dynamic_cast only for debug purposes.
     */
    template <typename _To, typename _From>
    _To *confident_downcast(_From *f)
    {
#ifdef NDEBUG
        return static_cast<_To*>(f);
#else
       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad confident_downcast");
       }

       return t;
#endif
    }

}

#endif /* INCLUDE_ODCORE_DOWNCAST_H_ */

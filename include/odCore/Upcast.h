/*
 * Upcast.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_UPCAST_H_
#define INCLUDE_ODCORE_UPCAST_H_

#include <odCore/Exception.h>

namespace od
{

    /**
     * An upcast which must succeed, and will throw otherwise.
     *
     * In contrast to od::confident_upcast(), this will always perform a dynamic_cast.
     */
    template <typename _To, typename _From>
    _To *upcast(_From *f)
    {
       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad upcast");
       }

       return t;
    }

    /**
     * A style of upcast for when the caller is confident that it will always succeed,
     * such as when casting to implementations of interfaces of which only one should
     * exist in the application (e.g. renderer, physics system...).
     *
     * In contrast to od::upcast(), this might actually do a static_cast for efficiency,
     * while using a dynamic_cast only for debug purposes.
     */
    template <typename _To, typename _From>
    _To *confident_upcast(_From *f)
    {
#ifdef NDEBUG
        return static_cast<_To*>(f);
#else
       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad confident_upcast");
       }

       return t;
#endif
    }

}

#endif /* INCLUDE_ODCORE_UPCAST_H_ */

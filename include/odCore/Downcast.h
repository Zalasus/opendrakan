/*
 * Downcast.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_DOWNCAST_H_
#define INCLUDE_ODCORE_DOWNCAST_H_

#include <memory>

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
        if(f == nullptr)
        {
            return nullptr;
        }

       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad downcast");
       }

       return t;
    }

    /**
     * @brief Overload of od::downcast for std::shared_ptr
     */
    template <typename _To, typename _From>
    std::shared_ptr<_To> *downcast(std::shared_ptr<_From> f)
    {
        if(f == nullptr)
        {
            return nullptr;
        }

       _To *t = dynamic_cast<_To*>(f.get());
       if(t == nullptr)
       {
           throw od::Exception("Bad downcast");
       }

       return std::shared_ptr<_To>(f, t); // aliasing constructor. returned pointer will own f but deref into t
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
        if(f == nullptr)
        {
            return nullptr;
        }

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

    /**
     * @brief Overload of od::confident_downcast for std::shared_ptr.
     */
    template <typename _To, typename _From>
    std::shared_ptr<_To> confident_downcast(std::shared_ptr<_From> f)
    {
        if(f == nullptr)
        {
            return nullptr;
        }

        _To *t;
#ifdef NDEBUG
        t = static_cast<_To*>(f.get());
#else
        t = dynamic_cast<_To*>(f.get());
        if(t == nullptr)
        {
            throw od::Exception("Bad confident_downcast");
        }
#endif
        return std::shared_ptr<_To>(f, t); // aliasing constructor. returned pointer will own f but deref into t
    }


    namespace detail
    {
        template <typename _From, typename _To, bool>
        struct StaticDowncastHelper
        {
            static _To *cast(_From *p);
        };

        template <typename _From, typename _To>
        struct StaticDowncastHelper<_From, _To, true>
        {
            static _To *cast(_From *p)
            {
                return static_cast<_To*>(p);
            }
        };

        template <typename _From, typename _To>
        struct StaticDowncastHelper<_From, _To, false>
        {
            static _To *cast(_From *p)
            {
                ((void)p);
                return nullptr;
            }
        };
    }

    /**
     * Cast function template for a downcast that evaluates to nullptr if the conversion is statically deemed incorrect.
     *
     * TODO: this can probably be constexpr
     */
    template <typename _From, typename _To>
    _To *static_downcast(_From *f)
    {
        return detail::StaticDowncastHelper<_From, _To, std::is_base_of<_To, _From>::value>::cast(f);
    }

}

#endif /* INCLUDE_ODCORE_DOWNCAST_H_ */

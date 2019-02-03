/*
 * Utils.h
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_UTILS_H_
#define INCLUDE_ODOSG_UTILS_H_

#include <odCore/Exception.h>

namespace odOsg
{

    template <typename _To, typename _From>
    _To *upcast(_From *f)
    {
        // TODO: might replace with static cast. for render objects, this should be safe.
        //   we'll never mix different renderers, anyway
       _To *t = dynamic_cast<_To*>(f);
       if(t == nullptr)
       {
           throw od::Exception("Bad upcast");
       }

       return t;
    }

}

#endif /* INCLUDE_ODOSG_UTILS_H_ */

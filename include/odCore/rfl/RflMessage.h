/*
 * RflMessage.h
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLMESSAGE_H_
#define INCLUDE_RFL_RFLMESSAGE_H_

#include <ostream>

namespace od
{

    enum class RflMessage
    {
        Off,
        On,
        Lock,
        Unlock,
        PlaySequence,
        BlowUp,
        Triggered,
        MoveToWaypoint,
        ImDead,
        Gib
    };


    std::ostream &operator<<(std::ostream &out, RflMessage message);

}


#endif /* INCLUDE_RFL_RFLMESSAGE_H_ */

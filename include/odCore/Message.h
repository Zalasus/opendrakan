/*
 * Message.h
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_MESSAGE_H_
#define INCLUDE_ODCORE_MESSAGE_H_

#include <ostream>

namespace od
{

    enum class Message
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


    std::ostream &operator<<(std::ostream &out, Message message);

}


#endif /* INCLUDE_ODCORE_MESSAGE_H_ */

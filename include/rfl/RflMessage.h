/*
 * RflMessage.h
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_RFLMESSAGE_H_
#define INCLUDE_RFL_RFLMESSAGE_H_


namespace odRfl
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

}


#endif /* INCLUDE_RFL_RFLMESSAGE_H_ */

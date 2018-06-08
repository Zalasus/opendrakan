/*
 * RflMessage.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: zal
 */

#include "rfl/RflMessage.h"

namespace odRfl
{

    std::ostream &operator<<(std::ostream &out, RflMessage message)
    {
        const char *msgStr = nullptr;

        switch(message)
        {
        case RflMessage::Off:
            msgStr = "Off";
            break;

        case RflMessage::On:
            msgStr = "On";
            break;

        case RflMessage::Lock:
            msgStr = "Lock";
            break;

        case RflMessage::Unlock:
            msgStr = "Unlock";
            break;

        case RflMessage::PlaySequence:
            msgStr = "Play Sequence";
            break;

        case RflMessage::BlowUp:
            msgStr = "Blow Up";
            break;

        case RflMessage::Triggered:
            msgStr = "Triggered";
            break;

        case RflMessage::MoveToWaypoint:
            msgStr = "Move to WayPoint";
            break;

        case RflMessage::ImDead:
            msgStr = " I'm Dead";
            break;

        case RflMessage::Gib:
            msgStr = " Gib";
            break;

        default:
            msgStr = "<unknown message>";
        }

        out << msgStr;

        return out;
    }

}



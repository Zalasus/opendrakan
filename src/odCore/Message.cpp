/*
 * Message.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#include <odCore/Message.h>

namespace od
{

    std::ostream &operator<<(std::ostream &out, Message message)
    {
        const char *msgStr = nullptr;

        switch(message)
        {
        case Message::Off:
            msgStr = "Off";
            break;

        case Message::On:
            msgStr = "On";
            break;

        case Message::Lock:
            msgStr = "Lock";
            break;

        case Message::Unlock:
            msgStr = "Unlock";
            break;

        case Message::PlaySequence:
            msgStr = "Play Sequence";
            break;

        case Message::BlowUp:
            msgStr = "Blow Up";
            break;

        case Message::Triggered:
            msgStr = "Triggered";
            break;

        case Message::MoveToWaypoint:
            msgStr = "Move to WayPoint";
            break;

        case Message::ImDead:
            msgStr = " I'm Dead";
            break;

        case Message::Gib:
            msgStr = " Gib";
            break;

        default:
            msgStr = "<unknown message>";
        }

        out << msgStr;

        return out;
    }

}


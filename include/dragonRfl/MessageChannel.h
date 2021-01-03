
#ifndef INCLUDE_DRAGONRFL_MESSAGECHANNEL_H_
#define INCLUDE_DRAGONRFL_MESSAGECHANNEL_H_

namespace dragonRfl
{

    /**
     * TODO: we could probably integrate the simple int-messages used by drakan into this, thus making these work across network basically for free.
     *  we'd just have to couple the message channels with the timeline since object-object messages must be tick-synchronized
     */
    enum class MessageChannel
    {
        HUMANCONTROL_CREATED
    };


}

#endif

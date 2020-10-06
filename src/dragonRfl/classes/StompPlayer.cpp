
#include <dragonRfl/classes/StompPlayer.h>

namespace dragonRfl
{

    StompPlayerFields::StompPlayerFields()
    : sequenceList({})
    , listPlayOrder(ListPlayOrder::IN_ORDER_ONCE)
    , initialState(PlayState::STOPPED)
    , messageToPlayNext(od::Message::PlaySequence)
    {
    }

    void StompPlayerFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("STOMP Player")
                (sequenceList, "Sequence List")
                (listPlayOrder, "List Play Order")
                (initialState, "Initial State")
                (messageToPlayNext, "Message To Play Next");
    }


    void StompPlayer_Sv::onLoaded()
    {
    }

    void StompPlayer_Sv::onSpawned()
    {
    }

    void StompPlayer_Sv::onDespawned()
    {
    }

}


#include <dragonRfl/classes/StompPlayer.h>

#include <cstdlib>

#include <odCore/LevelObject.h>

#include <odCore/db/Sequence.h>

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


    StompPlayer_Sv::StompPlayer_Sv()
    : mLastPlayedSequence(-1)
    {
    }

    void StompPlayer_Sv::onLoaded()
    {
        if(getLevelObject().getClass() != nullptr)
        {
            mFields.sequenceList.fetchAssets(getLevelObject().getClass()->getAssetProvider(), true);
            mPlayers.resize(mFields.sequenceList.getAssetCount());
        }
    }

    void StompPlayer_Sv::onSpawned()
    {
        if(mFields.initialState == StompPlayerFields::PlayState::PLAY)
        {
            _playNextSequence();
        }
    }

    void StompPlayer_Sv::onDespawned()
    {
    }

    void StompPlayer_Sv::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        if(message == mFields.messageToPlayNext)
        {
            _playNextSequence();
        }
    }

    void StompPlayer_Sv::onUpdate(float relTime)
    {
        for(auto &player : mPlayers)
        {
            bool stillRunning = player.update(relTime);
            if(!stillRunning)
            {
                getLevelObject().setEnableUpdate(false);
            }
        }
    }

    void StompPlayer_Sv::_playNextSequence()
    {
        int sequenceToPlay = -1;
        switch(mFields.listPlayOrder)
        {
        case StompPlayerFields::ListPlayOrder::IN_ORDER_ONCE:
            sequenceToPlay = mLastPlayedSequence + 1;
            break;

        case StompPlayerFields::ListPlayOrder::IN_ORDER_LOOP:
            sequenceToPlay = mLastPlayedSequence + 1;
            if(sequenceToPlay >= static_cast<int>(mFields.sequenceList.getAssetCount()))
            {
                sequenceToPlay = 0;
            }
            break;

        case StompPlayerFields::ListPlayOrder::RANDOMLY:
            sequenceToPlay = std::rand() % mFields.sequenceList.getAssetCount();
            break;

        case StompPlayerFields::ListPlayOrder::ALL_AT_ONCE:
            // unimplemented
            break;
        }

        if(sequenceToPlay >= 0 && sequenceToPlay < static_cast<int>(mFields.sequenceList.getAssetCount()))
        {
            auto sequence = mFields.sequenceList.getAsset(sequenceToPlay);
            if(sequence != nullptr)
            {
                Logger::verbose() << "Playing sequence '" << sequence->getName() << "'";
                mPlayers.back().setSequence(getLevelObject().getLevel(), *sequence);
                getLevelObject().setEnableUpdate(true);

            }else
            {
                Logger::error() << "Can't play sequence " << mFields.sequenceList.getAssetRef(sequenceToPlay) << " (invalid asset ref)";
            }

            mLastPlayedSequence = sequenceToPlay;
        }
    }

}

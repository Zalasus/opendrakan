
#ifndef INCLUDE_RFL_DRAGON_STOMPPLAYER_H_
#define INCLUDE_RFL_DRAGON_STOMPPLAYER_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>
#include <odCore/rfl/DummyClass.h>

#include <odCore/physics/Handles.h>

namespace dragonRfl
{

    struct StompPlayerFields : public odRfl::FieldBundle
    {

        enum class ListPlayOrder
        {
            IN_ORDER_ONCE,
            IN_ORDER_LOOP,
            RANDOMLY,
            ALL_AT_ONCE
        };

        enum class PlayState
        {
            STOPPED,
            PLAY
        };

        StompPlayerFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::SequenceRefArray              sequenceList;
        odRfl::EnumImpl<ListPlayOrder, 0, 3> listPlayOrder;
        odRfl::EnumImpl<PlayState, 0, 1>     initialState;
        odRfl::EnumMessage                   messageToPlayNext;
    };


    class StompPlayer_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<StompPlayer_Sv>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onSpawned() override;
        virtual void onDespawned() override;


    private:

        StompPlayerFields mFields;

    };

    using StomPlayerFactory = odRfl::DefaultClassFactory<StompPlayerFields, odRfl::DummyClass, StompPlayer_Sv>;

    OD_DEFINE_CLASS(StompPlayer, 0x0033, "System", "STOMP Player", StomPlayerFactory);

}

#endif /* INCLUDE_RFL_DRAGON_STATICLIGHT_H_ */

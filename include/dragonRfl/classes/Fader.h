
#ifndef INCLUDE_RFL_DRAGON_FADER_H_
#define INCLUDE_RFL_DRAGON_FADER_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/DummyClass.h>

#include <dragonRfl/classes/common/Enums.h>

namespace dragonRfl
{

    struct FaderFields final : public odRfl::FieldBundle
    {
        FaderFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::Float  fadeInTime;
        odRfl::Float  fadedTime;
        odRfl::Float  fadeOutTime;
        EnumStartMode startMode;
        odRfl::Color  color;
    };


    class Fader_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Fader_Cl>
    {
    public:

        Fader_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onSpawned() override;
        virtual void onUpdate(float relTime) override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        enum class FadePhase
        {
            NOT_TRIGGERED,
            FADE_IN,
            FADED,
            FADE_OUT
        };

        FaderFields mFields;

        float mTime;
        FadePhase mPhase;
    };


    using FaderFactory = odRfl::DefaultClassFactory<FaderFields, Fader_Cl, odRfl::DummyClass>;


    OD_DEFINE_CLASS(Fader, 0x0088, "Special Effect", "Fader", FaderFactory);

}

#endif
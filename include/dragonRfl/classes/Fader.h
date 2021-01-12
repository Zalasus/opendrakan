
#ifndef INCLUDE_RFL_DRAGON_FADER_H_
#define INCLUDE_RFL_DRAGON_FADER_H_

#include <odCore/render/Handle.h>

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/DummyClass.h>

#include <odCore/state/StateBundle.h>

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


    struct FaderStates final : public odState::StateBundle<FaderStates>
    {
        OD_BEGIN_STATE_LIST()
            OD_STATE(fade, odState::StateFlags::LERPED)
        OD_END_STATE_LIST()

        odState::State<float> fade;
    };


    class Fader_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Fader_Sv>
    {
    public:

        Fader_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }
        virtual odState::StateBundleBase *getExtraStates() override { return &mStates; }

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
        FaderStates mStates;

        float mTime;
        FadePhase mPhase;
    };


    class Fader_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<Fader_Cl>
    {
    public:

        Fader_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }
        virtual odState::StateBundleBase *getExtraStates() override { return &mStates; }

        virtual void onSpawned() override;
        virtual void onVisibilityChanged(bool v) override;
        virtual void onExtraStatesChanged() override;


    private:

        FaderFields mFields;
        FaderStates mStates;

        std::shared_ptr<odRender::Handle> mHandleToFade;
    };


    using FaderFactory = odRfl::DefaultClassFactory<FaderFields, Fader_Cl, Fader_Sv>;


    OD_DEFINE_CLASS(Fader, 0x0088, "Special Effect", "Fader", FaderFactory);

}

#endif

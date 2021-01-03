
#ifndef INCLUDE_RFL_DRAGON_MORPHINGOBJECT_H_
#define INCLUDE_RFL_DRAGON_MORPHINGOBJECT_H_

namespace dragonRfl
{

    struct MorphingObject_Fields final : public odRfl::FieldBundle
    {
        MorphingObject_Fields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::ModelRefArray morphingModels;
        odRfl::FloatArray    morphTimeToNextModel;
        odRfl::EnumYesNo     looping;
        odRfl::EnumYesNo     startWhenCreated;
        odRfl::EnumYesNo     destructWhenEndIsReached; // sic
        odRfl::SoundRefArray sounds; // (chosen randomly)
        odRfl::EnumYesNo     soundLoops;
        odRfl::Int           fadeoutFrame; // (0 = don't fade)
        odRfl::EnumYesNo     useModelBounds;
    };


    class MorphingObject_Sv final : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<MorphingObject_Sv>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onMessageReceived() override { bounce(); }


    private:

        MorphingObject_Fields mFields;

    };


    class MorphingObject_Cl final : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<MorphingObject_Cl>
    {
    public:

        virtual void onRemoteMessageReceived() override;


    private:

        MorphingObject_Fields mFields;

    };


    OD_DEFINE_CLASS(MorphingObject, 0x0063, "System", "Ground Object", MorphingObject_Factory);

}

#endif

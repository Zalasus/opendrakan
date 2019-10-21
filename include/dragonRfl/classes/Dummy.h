/*
 * @file Dummy.h
 * @date 8 Oct 2019
 * @author zal
 */

#ifndef INCLUDE_DRAGONRFL_CLASSES_DUMMY_H_
#define INCLUDE_DRAGONRFL_CLASSES_DUMMY_H_

#include <odCore/rfl/Class.h>

namespace dragonRfl
{

    class DummyFields : public odRfl::FieldBundle
    {
    public:

        DummyFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


    protected:

        float           mDummyFloat;
        int             mDummyInt;

    };

    class Dummy : public odRfl::ClassBase, public odRfl::Spawnable, public DummyFields
    {
    public:

        virtual Spawnable *asSpawnable() override final;

        // shared code goes here

    };


    class Dummy_Cl : public odRfl::ClientClassImpl, public Dummy
    {
    public:

        Dummy_Cl(od::Client &client);

    };


    class Dummy_Sv : public odRfl::ServerClassImpl, public Dummy
    {
    public:

        Dummy_Sv(od::Server &server);

    };

}

ODRFL_DEFINE_CLASS(dragonRfl::Dummy, 0xffff, "OD-Specific", "Dummy");
ODRFL_DEFINE_CLASS_FIELDS(dragonRfl::Dummy, dragonRfl::DummyFields);
ODRFL_DEFINE_CLASS_IMPL_CLIENT(dragonRfl::Dummy, dragonRfl::Dummy_Cl);
ODRFL_DEFINE_CLASS_IMPL_SERVER(dragonRfl::Dummy, dragonRfl::Dummy_Sv);

#endif /* INCLUDE_DRAGONRFL_CLASSES_DUMMY_H_ */

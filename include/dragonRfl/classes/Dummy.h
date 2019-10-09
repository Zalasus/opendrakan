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

    class DummyFields : public odRfl::FieldProbeable
    {
    public:

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


    protected:

        float           mDummyFloat;
        int             mDummyInt;

    };

    class Dummy : public odRfl::ClassBase
    {
    public:

        Dummy();

        virtual void probeFields(odRfl::FieldProbe &probe) override final;


    protected:

        float           mDummyFloat;
        int             mDummyInt;
    };


    class Dummy_Cl : public Dummy, public odRfl::ClientClassImpl
    {
    public:



    };


    class Dummy_Sv : public Dummy, public odRfl::ServerClassImpl
    {
    public:


    };

}

ODRFL_DEFINE_CLASS(dragonRfl::Dummy, 0xffff, "OD-Specific", "Dummy");
ODRFL_DEFINE_CLASS_IMPL_CLIENT(dragonRfl::Dummy, dragonRfl::Dummy_Cl);
ODRFL_DEFINE_CLASS_IMPL_SERVER(dragonRfl::Dummy, dragonRfl::Dummy_Sv);

#endif /* INCLUDE_DRAGONRFL_CLASSES_DUMMY_H_ */

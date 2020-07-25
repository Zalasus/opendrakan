
#ifndef INCLUDE_ODCORE_RFL_DUMMYCLASS_H_
#define INCLUDE_ODCORE_RFL_DUMMYCLASS_H_

#include <odCore/rfl/Class.h>

namespace odRfl
{

    /**
     * @brief A dummy field bundle with no fields.
     */
    class DummyFields final : public FieldBundle
    {
    public:

        virtual void probeFields(odRfl::FieldProbe &probe) override { }

    };

    /**
     * @brief Dummy class that does nothing, has no fields and can be used everywhere.
     */
    class DummyClass final : public ServerClass, public ClientClass, public SpawnableClass, public ClassImpl<TrackingCamera_Cl>
    {
    public:

        virtual odRfl::FieldBundle &getFields() override { return mFields; }


    private:

        DummyFields mFields;

    };

    
    using DummyClassFactory = odRfl::DefaultClassFactory<DummyFields, DummyClass, DummyClass>;

}

#endif

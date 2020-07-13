/*
 * VisibilityToggler.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_
#define INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>

namespace dragonRfl
{

    class VisibilityTogglerFields final : public odRfl::FieldBundle
    {
    public:

        enum class TriggerMode
        {
            ToggleVisibility,
            DependsOnMessage
        };

        VisibilityTogglerFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::EnumImpl<TriggerMode, 0, 1>  triggerMode;
        odRfl::EnumMessage                  showMessage;
        odRfl::EnumMessage                  hideMessage;

    };

    class VisibilityToggler_Sv : public odRfl::ServerClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<VisibilityToggler_Sv>
    {
    public:

        VisibilityToggler_Sv();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        VisibilityTogglerFields mFields;

    };

    class VisibilityToggler_Cl : public odRfl::ClientClass, public odRfl::SpawnableClass, public odRfl::ClassImpl<VisibilityToggler_Sv>
    {
    public:

        VisibilityToggler_Cl();

        virtual odRfl::FieldBundle &getFields() override { return mFields; }

        virtual void onLoaded() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    private:

        VisibilityTogglerFields mFields;

    };

    OD_DEFINE_CLASS(VisibilityToggler, 0x0079, "System", "Visibility Toggler", odRfl::DefaultClassFactory<VisibilityTogglerFields, VisibilityToggler_Cl, VisibilityToggler_Sv>);

}


#endif /* INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_ */

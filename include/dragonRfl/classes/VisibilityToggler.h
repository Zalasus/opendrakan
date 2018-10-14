/*
 * VisibilityToggler.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_
#define INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace dragonRfl
{

    class DragonRfl;

    class VisibilityToggler : public odRfl::RflClass
    {
    public:

        enum class TriggerMode
        {
            ToggleVisibility,
            DependsOnMessage
        };

        VisibilityToggler(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onLoaded(od::LevelObject &obj) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, odRfl::RflMessage message) override;


    protected:

        odRfl::RflEnumImpl<TriggerMode, 0, 1>  mTriggerMode;
        odRfl::RflEnumMessage                  mShowMessage;
        odRfl::RflEnumMessage                  mHideMessage;

    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0079, "System", "Visibility Toggler", dragonRfl::VisibilityToggler);

#endif /* INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_ */

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

    class VisibilityToggler : public odRfl::LevelObjectClassBase
    {
    public:

        enum class TriggerMode
        {
            ToggleVisibility,
            DependsOnMessage
        };

        VisibilityToggler();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onLoaded() override;
        virtual void onMessageReceived(od::LevelObject &sender, od::Message message) override;


    protected:

        odRfl::EnumImpl<TriggerMode, 0, 1>  mTriggerMode;
        odRfl::EnumMessage                  mShowMessage;
        odRfl::EnumMessage                  mHideMessage;

    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::VisibilityToggler, 0x0079, "System", "Visibility Toggler");

#endif /* INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_ */

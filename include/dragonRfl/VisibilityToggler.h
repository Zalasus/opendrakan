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

namespace od
{

    class VisibilityToggler : public RflClass
    {
    public:

        enum class TriggerMode
        {
            ToggleVisibility,
            DependsOnMessage
        };

        VisibilityToggler();

        virtual void probeFields(FieldProbe &probe) override;

        virtual void onLoaded(LevelObject &obj) override;
        virtual void onMessageReceived(LevelObject &obj, LevelObject &sender, RflMessage message) override;


    protected:

        RflEnumImpl<TriggerMode, 0, 1>  mTriggerMode;
        RflEnumMessage                  mShowMessage;
        RflEnumMessage                  mHideMessage;

    };

}

#endif /* INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_ */

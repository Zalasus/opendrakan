/*
 * VisibilityToggler.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_
#define INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
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

        virtual void probeFields(RflFieldProbe &probe) override;

        virtual void onLoaded(od::Engine &e, od::LevelObject *obj) override;
        virtual void onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message) override;


    protected:

        RflEnumImpl<TriggerMode, 0, 1>  mTriggerMode;
        RflEnumMessage                  mShowMessage;
        RflEnumMessage                  mHideMessage;

    };

}

#endif /* INCLUDE_RFL_DRAGON_VISIBILITYTOGGLER_H_ */

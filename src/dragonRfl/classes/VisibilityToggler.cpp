/*
 * VisibilityToggler.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/classes/VisibilityToggler.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

    VisibilityToggler::VisibilityToggler()
    : mTriggerMode(TriggerMode::DependsOnMessage)
    , mShowMessage(od::Message::Off)
    , mHideMessage(od::Message::Off)
    {
    }

    void VisibilityToggler::probeFields(odRfl::FieldProbe &probe)
    {
        probe("")
                (mTriggerMode, "Trigger Mode")
                (mShowMessage, "Show Message")
                (mHideMessage, "Hide Message");
    }

    void VisibilityToggler::onLoaded()
    {
        auto &obj = getLevelObject();

        obj.setSpawnStrategy(od::SpawnStrategy::Always);
        obj.setObjectType(od::LevelObjectType::Detector);
    }

    void VisibilityToggler::onMessageReceived(od::LevelObject &sender, od::Message message)
    {
        const std::vector<od::LevelObject*> &linkedObjects = getLevelObject().getLinkedObjects();

        if(mTriggerMode == TriggerMode::ToggleVisibility)
        {
            for(auto it = linkedObjects.begin(); it != linkedObjects.end(); ++it)
            {
                bool visible = (*it)->isVisible();
                (*it)->setVisible(!visible);
            }

        }else if(mTriggerMode == TriggerMode::DependsOnMessage)
        {
            bool desiredVisibility;
            if(message == mShowMessage)
            {
                desiredVisibility = true;

            }else if(message == mHideMessage)
            {
                desiredVisibility = false;

            }else
            {
                return; // ignore message
            }

            for(auto it = linkedObjects.begin(); it != linkedObjects.end(); ++it)
            {
                (*it)->setVisible(desiredVisibility);
            }
        }
    }

}

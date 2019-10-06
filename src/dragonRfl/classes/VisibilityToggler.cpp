/*
 * VisibilityToggler.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include <dragonRfl/classes/VisibilityToggler.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>

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
        const auto &linkedObjects = getLevelObject().getLinkedObjectIndices();

        for(auto index : linkedObjects)
        {
            od::LevelObject *obj = getLevelObject().getLevel().getLevelObjectByIndex(index);
            if(obj == nullptr) continue;

            if(mTriggerMode == TriggerMode::ToggleVisibility)
            {

                obj->setVisible(!obj->isVisible());

            }else if(mTriggerMode == TriggerMode::DependsOnMessage)
            {
                if(message == mShowMessage)
                {
                    obj->setVisible(true);

                }else if(message == mHideMessage)
                {
                    obj->setVisible(false);

                }else
                {
                    return; // ignore message
                }
            }
        }
    }

}

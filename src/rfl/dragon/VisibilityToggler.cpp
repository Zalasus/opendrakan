/*
 * VisibilityToggler.cpp
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#include "rfl/dragon/VisibilityToggler.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"

namespace odRfl
{

    VisibilityToggler::VisibilityToggler()
    : mTriggerMode(TriggerMode::DependsOnMessage)
    , mShowMessage(RflMessage::Off)
    , mHideMessage(RflMessage::Off)
    {
    }

    void VisibilityToggler::probeFields(RflFieldProbe &probe)
    {
        probe("")
                (mTriggerMode, "Trigger Mode")
                (mShowMessage, "Show Message")
                (mHideMessage, "Hide Message");
    }

    void VisibilityToggler::onLoaded(od::Engine &e, od::LevelObject *obj)
    {
        if(obj == nullptr)
        {
            Logger::warn() << "Visibility toggler used without level object";
            return;
        }

        obj->setSpawnStrategy(od::SpawnStrategy::Always);
    }

    void VisibilityToggler::onMessageReceived(od::LevelObject &obj, od::LevelObject &sender, RflMessage message)
    {
        const std::vector<osg::ref_ptr<od::LevelObject>> &linkedObjects = obj.getLinkedObjects();

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


    OD_REGISTER_RFL_CLASS(0x0079, "Visibility Toggler", VisibilityToggler);

}

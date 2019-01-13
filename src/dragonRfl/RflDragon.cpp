/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

#include <odCore/Engine.h>

#include <odCore/input/InputManager.h>

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/db/DbManager.h>

#include <dragonRfl/Actions.h>

#include <dragonRfl/gui/DragonGui.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>


namespace dragonRfl
{

    DragonRfl::DragonRfl(od::Engine &engine)
    : AutoRegisteringRfl<DragonRfl>(engine)
    , mLocalPlayer(nullptr)
    {
    }

    DragonRfl::~DragonRfl()
    {
    }

    void DragonRfl::onStartup()
    {
        od::Engine &engine = getEngine();

        if(engine.getRenderer() != nullptr)
        {
            mGui = std::make_unique<DragonGui>(engine);
            engine.getInputManager().setGui(mGui.get());
        }

        if(!engine.hasInitialLevelOverride())
        {
            od::FilePath initialLevel(mGui->getUserInterfaceProperties()->mIntroLevelFilename);
            engine.loadLevel(initialLevel.adjustCase());
        }

        odInput::InputManager &im = engine.getInputManager();

        auto actionHandler = std::bind(&DragonRfl::_handleAction, this, std::placeholders::_1, std::placeholders::_2);
        auto mmAction = im.getOrCreateAction(Action::Main_Menu);
        mmAction->setRepeatable(false);
        mmAction->addCallback(actionHandler);
        mmAction->bindToKey(odInput::Key::Return);
        mMenuAction = mmAction;
    }

    void DragonRfl::onMenuToggle(bool newMode)
    {
    }

    void DragonRfl::_handleAction(odInput::ActionHandle<Action> *action, odInput::InputEvent event)
    {
        switch(action->getAction())
        {
        case Action::Main_Menu:
            mGui->setMenuMode(!mGui->isMenuMode());
            break;

        default:
            break;
        }
    }


    OD_REGISTER_RFL(DragonRfl);

}

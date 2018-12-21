/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

#include <odCore/Engine.h>
#include <odCore/rfl/PrefetchProbe.h>
#include <odCore/db/DbManager.h>

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
            mGui.reset(new DragonGui(engine));
        }

        if(!engine.hasInitialLevelOverride())
        {
            od::FilePath initialLevel(mGui->getUserInterfaceProperties()->mIntroLevelFilename);
            engine.loadLevel(initialLevel.adjustCase());
        }
    }

    void DragonRfl::onMenuToggle(bool newMode)
    {
    }


    OD_REGISTER_RFL(DragonRfl);

}

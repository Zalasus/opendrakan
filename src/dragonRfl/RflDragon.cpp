/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

#include <odCore/Engine.h>
#include <odCore/gui/GuiManager.h>
#include <odCore/rfl/PrefetchProbe.h>
#include <odCore/db/DbManager.h>
#include <dragonRfl/gui/Cursor.h>
#include <dragonRfl/classes/UserInterfaceProperties.h>

#define OD_INTERFACE_DB_PATH "Common/Interface/Interface.db"

namespace dragonRfl
{

    DragonRfl::DragonRfl(od::Engine &engine)
    : AutoRegisteringRfl<DragonRfl>(engine)
    , mInterfaceDb(nullptr)
    {
    }

    void DragonRfl::onStartup()
    {
        od::Engine &engine = getEngine();

        mInterfaceDb = &engine.getDbManager().loadDb(od::FilePath(OD_INTERFACE_DB_PATH, engine.getEngineRootDir()).adjustCase());

        // retrieve UserInterfaceProperties object
        if(mInterfaceDb->getClassFactory() == nullptr)
        {
            throw od::Exception("Can not initialize user interface. Interface.db has no class container");
        }

        odRfl::RflClassId uiPropsClassId = odRfl::RflClassTraits<UserInterfaceProperties>::classId();
        od::RecordId id = mInterfaceDb->getClassFactory()->findFirstClassOfType(uiPropsClassId);
        if(id == odDb::AssetRef::NULL_REF.assetId)
        {
            throw od::Exception("Can not initialize user interface. Interface class container has no User Interface Properties class");
        }

        osg::ref_ptr<odDb::Class> uiPropsClass = mInterfaceDb->getClass(id);
        std::unique_ptr<odRfl::RflClass> uiPropsInstance = uiPropsClass->makeInstance();
        mUserInterfacePropertiesInstance.reset(dynamic_cast<UserInterfaceProperties*>(uiPropsInstance.release()));
        if(mUserInterfacePropertiesInstance == nullptr)
        {
            throw od::Exception("Could not cast or instantiate User Interface Properties instance");
        }
        mUserInterfacePropertiesInstance->onLoaded(engine);

        odRfl::PrefetchProbe probe(*mInterfaceDb);
        mUserInterfacePropertiesInstance->probeFields(probe);


        if(!engine.hasInitialLevelOverride())
        {
            od::FilePath initialLevel(mUserInterfacePropertiesInstance->mIntroLevelFilename);
            engine.loadLevel(initialLevel.adjustCase());
        }


        odGui::GuiManager &gm = engine.getGuiManager();

        mMainMenu = new MainMenu(gm, mUserInterfacePropertiesInstance.get());
        mMainMenu->setVisible(gm.isMenuMode());
        gm.addWidget(mMainMenu);

        osg::ref_ptr<Cursor> cursor = new Cursor(gm);
        cursor->setPosition(0.5, 0.5);
        engine.getGuiManager().setCursorWidget(cursor);
    }

    void DragonRfl::onMenuToggle(bool newMode)
    {
        mMainMenu->setVisible(newMode);
    }


    OD_REGISTER_RFL(DragonRfl);

}

/*
 * DragonGui.cpp
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#include <dragonRfl/gui/DragonGui.h>

#include <odCore/Engine.h>
#include <odCore/db/DbManager.h>
#include <odCore/rfl/PrefetchProbe.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>


#define OD_INTERFACE_DB_PATH "Common/Interface/Interface.db"
#define OD_DRAGONRRC_PATH "Dragon.rrc"

namespace dragonRfl
{

    DragonGui::DragonGui(od::Engine &engine)
    : mRrcFile(od::FilePath(OD_DRAGONRRC_PATH, engine.getEngineRootDir()).adjustCase())
    , mRrcTextureFactory(*this, mRrcFile, engine)
    , mInterfaceDb(nullptr)
    {
        od::FilePath interfaceDbPath(OD_INTERFACE_DB_PATH, engine.getEngineRootDir());
        mInterfaceDb = &engine.getDbManager().loadDb(interfaceDbPath.adjustCase());

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

        odDb::AssetPtr<odDb::Class> uiPropsClass = mInterfaceDb->getClass(id);
        std::unique_ptr<odRfl::RflClass> uiPropsInstance = uiPropsClass->makeInstance();
        mUserInterfaceProperties.reset(dynamic_cast<UserInterfaceProperties*>(uiPropsInstance.release()));
        if(mUserInterfaceProperties == nullptr)
        {
            throw od::Exception("Could not cast or instantiate User Interface Properties instance");
        }
        mUserInterfaceProperties->onLoaded(engine);

        odRfl::PrefetchProbe probe(*mInterfaceDb);
        mUserInterfaceProperties->probeFields(probe);
    }

    DragonGui::~DragonGui()
    {
    }

    odDb::Texture *DragonGui::getTexture(od::RecordId recordId)
    {
        return mRrcTextureFactory.getAsset(recordId).release();
    }

}

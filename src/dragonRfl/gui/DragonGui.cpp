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

#include <dragonRfl/gui/Cursor.h>
#include <dragonRfl/gui/MainMenu.h>
#include <dragonRfl/gui/HealthIndicator.h>

#define OD_INTERFACE_DB_PATH "Common/Interface/Interface.db"
#define OD_DRAGONRRC_PATH "Dragon.rrc"

namespace dragonRfl
{

    DragonGui::DragonGui(od::Engine &engine)
    : odGui::Gui(*engine.getRenderer())
    , mEngine(engine)
    , mRrcFile(od::FilePath(OD_DRAGONRRC_PATH, engine.getEngineRootDir()).adjustCase())
    , mRrcTextureFactory(*this, mRrcFile)
    , mInterfaceDb(nullptr)
    {
        if(engine.getRenderer() == nullptr)
        {
            Logger::warn() << "Created DragonGui using Engine without renderer. Prepare for chaos";
        }

        od::FilePath interfaceDbPath(OD_INTERFACE_DB_PATH, engine.getEngineRootDir());
        mInterfaceDb = &engine.getDbManager().loadDb(interfaceDbPath.adjustCase());

        // retrieve UserInterfaceProperties object
        if(mInterfaceDb->getClassFactory() == nullptr)
        {
            throw od::Exception("Can not initialize user interface. Interface.db has no class container");
        }

        odRfl::ClassId uiPropsClassId = odRfl::ClassTraits<UserInterfaceProperties>::classId();
        od::RecordId id = mInterfaceDb->getClassFactory()->findFirstClassOfType(uiPropsClassId);
        if(id == odDb::AssetRef::NULL_REF.assetId)
        {
            throw od::Exception("Can not initialize user interface. Interface class container has no User Interface Properties class");
        }

        od::RefPtr<odDb::Class> uiPropsClass = mInterfaceDb->getClass(id);
        std::unique_ptr<odRfl::ClassBase> uiPropsInstance = uiPropsClass->makeInstance(engine.getRflManager());
        mUserInterfaceProperties.reset(dynamic_cast<UserInterfaceProperties*>(uiPropsInstance.release()));
        if(mUserInterfaceProperties == nullptr)
        {
            throw od::Exception("Could not cast or instantiate User Interface Properties instance");
        }
        mUserInterfaceProperties->onLoaded();

        odRfl::PrefetchProbe probe(*mInterfaceDb);
        mUserInterfaceProperties->probeFields(probe);

        auto cursor = od::make_refd<Cursor>(*this);
        setCursorWidget(cursor);

        setCursorPosition(glm::vec2(0, 0));

        auto healthIndicator = od::make_refd<HealthIndicator>(*this);
        healthIndicator->setOrigin(odGui::WidgetOrigin::BottomLeft);
        healthIndicator->setPosition(0, 1);
        addWidget(healthIndicator);

        mMainMenu = od::make_refd<MainMenu>(*this, mUserInterfaceProperties.get());
        addWidget(mMainMenu);

        setMenuMode(false);
    }

    DragonGui::~DragonGui()
    {
    }

    std::string DragonGui::localizeString(const std::string &s)
    {
        if(s.size() < 8 || s[0] != '<' || s[1] != '0' || s[2] != 'x' || s[7] != '>') // probably still the fastest method
        {
            return s;
        }

        od::RecordId stringId;
        std::istringstream iss(s.substr(3, 4));
        iss >> std::hex >> stringId;
        if(iss.fail())
        {
            return s;
        }

        Logger::debug() << "Localized string " << std::hex << stringId << " requested from RRC";

        try
        {
            return getStringById(stringId);

        }catch(od::NotFoundException &e)
        {
        }

        // string not found. return unlocalized one
        return s.substr(8, std::string::npos);
    }

    std::string DragonGui::getStringById(od::RecordId stringId)
    {
        // was string cached? if yes, no need to load and decrypt it again
        auto cacheIt = mLocalizedStringCache.find(stringId);
        if(cacheIt != mLocalizedStringCache.end())
        {
            return cacheIt->second;
        }

        // string was not cached. load and decrypt it
        auto cursor = mRrcFile.getFirstRecordOfTypeId(od::SrscRecordType::LOCALIZED_STRING, stringId);
        if(!cursor.isValid())
        {
            std::ostringstream oss;
            oss << "String with ID 0x" << std::hex << stringId << std::dec << " not found";

            throw od::NotFoundException(oss.str());
        }

        auto dirIt = cursor.getDirIterator();

        if(dirIt->dataSize > 255)
        {
            throw od::Exception("String buffer too small");
        }

        std::istream &in = mRrcFile.getStreamForRecord(dirIt);

        char str[256] = {0}; // should be big enough. might alloc it dynamically but can't be bothered right now
        in.read(str, dirIt->dataSize);
        size_t readBytes = in.gcount();

        _decryptString(str, readBytes);

        // TODO: Transcode from Latin-1 or what you got to UTF-8

        std::string decryptedString(str);
        mLocalizedStringCache.insert(std::make_pair(stringId, decryptedString));

        return std::move(decryptedString);
    }

    od::RefPtr<odDb::Texture> DragonGui::getTexture(od::RecordId recordId)
    {
        return mRrcTextureFactory.getAsset(recordId);
    }

    void DragonGui::onMenuModeChanged()
    {
        if(mMainMenu != nullptr)
        {
            mMainMenu->setVisible(isMenuMode());
        }
    }

    void DragonGui::_decryptString(char * const str, const size_t len)
    {
        uint32_t key = 0x5FDD390D;

        for(size_t i = 0; i < len; ++i)
        {
            str[i] ^= key & 0xFF;
            key = (key<<3) | (key>>(32-3));
        }
    }

}

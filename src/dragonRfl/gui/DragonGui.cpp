/*
 * DragonGui.cpp
 *
 *  Created on: 11 Dec 2018
 *      Author: zal
 */

#include <dragonRfl/gui/DragonGui.h>

#include <odCore/Client.h>

#include <odCore/rfl/PrefetchProbe.h>

#include <odCore/db/DbManager.h>

#include <odCore/gui/Quad.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>

#include <dragonRfl/gui/Cursor.h>
#include <dragonRfl/gui/MainMenu.h>
#include <dragonRfl/gui/HealthIndicator.h>

#define OD_INTERFACE_DB_PATH "Common/Interface/Interface.db"
#define OD_DRAGONRRC_PATH "Dragon.rrc"

namespace dragonRfl
{

    DragonGui::DragonGui(od::Client &client)
    : odGui::Gui(client.getRenderer())
    , mClient(client)
    , mRrcFile(od::FilePath(OD_DRAGONRRC_PATH, client.getEngineRootDir()).adjustCase())
    , mRrcTextureFactory(*this, mRrcFile)
    , mInterfaceDb(nullptr)
    {
        od::FilePath interfaceDbPath(OD_INTERFACE_DB_PATH, client.getEngineRootDir());
        mInterfaceDb = client.getDbManager().loadDb(interfaceDbPath.adjustCase());

        // retrieve UserInterfaceProperties object
        if(mInterfaceDb->getClassFactory() == nullptr)
        {
            throw od::Exception("Can not initialize user interface. Interface.db has no class container");
        }

        odRfl::ClassId uiPropsClassId = UserInterfaceProperties::classId();
        od::RecordId id = mInterfaceDb->getClassFactory()->findFirstClassOfType(uiPropsClassId);
        if(id == odDb::AssetRef::NULL_REF.assetId)
        {
            throw od::Exception("Can not initialize user interface. Interface class container has no User Interface Properties class");
        }

        std::shared_ptr<odDb::Class> uiPropsClass = mInterfaceDb->getClass(id);
        assert(uiPropsClass != nullptr); // this should not be null, given that the DB just gave us the ID
        uiPropsClass->fillFields(mUserInterfaceProperties);

        odRfl::PrefetchProbe probe(*mInterfaceDb);
        mUserInterfaceProperties.probeFields(probe);

        auto cursor = std::make_shared<Cursor>(*this);
        setCursorWidget(cursor);

        setCursorPosition(glm::vec2(0, 0));

        auto healthIndicator = std::make_shared<HealthIndicator>(*this);
        healthIndicator->setOrigin(odGui::WidgetOrigin::BottomLeft);
        healthIndicator->setPosition({0, 1});
        addWidget(healthIndicator);

        mMainMenu = std::make_shared<MainMenu>(*this, &mUserInterfaceProperties);
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

    std::shared_ptr<odDb::Texture> DragonGui::getTexture(od::RecordId recordId)
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

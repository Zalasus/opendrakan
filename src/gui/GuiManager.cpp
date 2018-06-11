/*
 * GuiManager.cpp
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#include "gui/GuiManager.h"

#include <sstream>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "SrscRecordTypes.h"
#include "Engine.h"

#define OD_INTERFACE_DB_PATH "Common/Interface/Interface.db"

namespace od
{


    GuiManager::GuiManager(Engine &engine, const FilePath &rrcPath)
    : mEngine(engine)
    , mRrcFile(FilePath(rrcPath))
    , mTextureFactory(*this, mRrcFile, mEngine)
    , mInterfaceDb(engine.getDbManager().loadDb(FilePath(OD_INTERFACE_DB_PATH, rrcPath.dir()).adjustCase()))
    {
    }

    std::string GuiManager::localizeString(const std::string &s)
    {
        if(s.size() < 8 || s[0] != '<' || s[1] != '0' || s[2] != 'x' || s[7] != '>') // probably still the fastest method
        {
            return s;
        }

        uint16_t stringId;
        std::istringstream iss(s.substr(3, 4));
        iss >> std::hex >> stringId;
        if(iss.fail())
        {
            return s;
        }

        Logger::debug() << "Localized string " << std::hex << stringId << " requested from RRC";

        auto dirIt = mRrcFile.getDirIteratorByTypeId(SrscRecordType::LOCALIZED_STRING, stringId);
        if(dirIt == mRrcFile.getDirectoryEnd())
        {
            Logger::debug() << "Could not find string in RRC. Leaving unlocalized";
            return s.substr(8, std::string::npos);
        }

        // now we got the string record, but i still don't know the key. return unlocalized one
        return s.substr(8, std::string::npos);
    }

    void GuiManager::dumpStrings()
    {
        uint8_t key[] = { 0x0D, 0x6A, 0x57, 0xBF, 0xFD, 0xEE, 0x74 };
        size_t keylength = sizeof(key);

        std::ofstream out("out/gui_strings.txt");
        if(out.fail())
        {
            throw IoException("Could not open 'out/gui_strings.txt' for dumping strings");
        }

        out << "Strings found in Dragon.rrc. Only first " << keylength << " characters decrypted, "
                " as we don't know more bytes of the key yet." << std::endl << std::endl;

        auto dirIt = mRrcFile.getDirIteratorByType(SrscRecordType::LOCALIZED_STRING);
        while(dirIt != mRrcFile.getDirectoryEnd())
        {
            std::istream &in = mRrcFile.getStreamForRecord(dirIt);

            char str[128] = {0};
            size_t readMax = std::min((uint32_t)keylength, dirIt->dataSize);
            in.read(str, readMax);
            size_t readBytes = in.gcount();

            for(size_t i = 0; i < keylength && i < readBytes; ++i)
            {
                str[i] ^= key[i];
            }

            out << "STR " << std::hex << std::setw(4) << dirIt->recordId << std::dec << ": " << str << std::endl;

            dirIt = mRrcFile.getDirIteratorByType(SrscRecordType::LOCALIZED_STRING, dirIt+1);
        }
    }

    void GuiManager::dumpTextures()
    {
        auto dirIt = mRrcFile.getDirIteratorByType(SrscRecordType::TEXTURE);
        while(dirIt != mRrcFile.getDirectoryEnd())
        {
            std::ostringstream oss;
            oss << "out/gui_tex_" << std::hex << dirIt->recordId << ".png";

            try
            {
                osg::ref_ptr<Texture> tex = getTexture(dirIt->recordId);
                tex->exportToPng(FilePath(oss.str()));

            }catch(UnsupportedException &e)
            {
                Logger::warn() << "Can't dump texture " << oss.str() << " (" << e.what() << ")";
            }

            dirIt = mRrcFile.getDirIteratorByType(SrscRecordType::TEXTURE, dirIt+1);
        }
    }

    Texture *GuiManager::getTextureByRef(const AssetRef &ref)
    {
        if(ref.dbIndex != 0)
        {
            throw Exception("Tried to use GUI Manager to load texture from outside RRC");
        }

        return getTexture(ref.assetId);
    }

    Texture *GuiManager::getTexture(RecordId recordId)
    {
        osg::ref_ptr<Texture> tex = mTextureFactory.getAsset(recordId);

        return tex.release();
    }


}
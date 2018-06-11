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

        RecordId stringId;
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

        }catch(NotFoundException &e)
        {
        }

        // string not found. return unlocalized one
        return s.substr(8, std::string::npos);
    }

    std::string GuiManager::getStringById(RecordId stringId)
    {
        auto dirIt = mRrcFile.getDirIteratorByTypeId(SrscRecordType::LOCALIZED_STRING, stringId);
        if(dirIt == mRrcFile.getDirectoryEnd())
        {
            std::ostringstream oss;
            oss << "String with ID 0x" << std::hex << stringId << std::dec << " not found";

            throw NotFoundException(oss.str());
        }

        if(dirIt->dataSize > 255)
        {
            throw Exception("String buffer too small");
        }

        std::istream &in = mRrcFile.getStreamForRecord(dirIt);

        char str[256] = {0}; // should be big enough. might alloc it dynamically but can't be bothered right now
        in.read(str, dirIt->dataSize);
        size_t readBytes = in.gcount();

        _decryptString(str, readBytes);

        // TODO: Transcode from Latin-1 or what you got to UTF-8

        return std::string(str);
    }

    void GuiManager::dumpStrings()
    {
        std::ofstream out("out/gui_strings.txt");
        if(out.fail())
        {
            throw IoException("Could not open 'out/gui_strings.txt' for dumping strings");
        }

        out << "Strings found in Dragon.rrc:" << std::endl << std::endl;

        auto dirIt = mRrcFile.getDirIteratorByType(SrscRecordType::LOCALIZED_STRING);
        while(dirIt != mRrcFile.getDirectoryEnd())
        {
            std::istream &in = mRrcFile.getStreamForRecord(dirIt);

            if(dirIt->dataSize > 255)
            {
                Logger::warn() << "String 0x" << dirIt->recordId << " too long for buffer. Skipping";

            }else
            {
                char str[256] = {0};
                in.read(str, dirIt->dataSize);
                size_t readBytes = in.gcount();
                _decryptString(str, readBytes);

                out << "STR " << std::hex << std::setw(4) << dirIt->recordId << std::dec << ": " << str << std::endl;
            }

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

    inline void GuiManager::_decryptString(char * const str, const size_t len)
    {
        uint32_t key = 0x5FDD390D;

        for(size_t i = 0; i < len; ++i)
        {
            str[i] ^= key & 0xFF;
            key = (key<<3) | (key>>(32-3));
        }
    }

}

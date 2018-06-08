/*
 * SoundFactory.cpp
 *
 *  Created on: 23 Apr 2018
 *      Author: zal
 */

#include "db/SoundFactory.h"

#include "db/Database.h"

namespace od
{

    SoundFactory::SoundFactory(const FilePath &sdbFilePath, Database &database)
    : AssetFactory<Sound>(sdbFilePath, database)
    {
    }

    osg::ref_ptr<Sound> SoundFactory::loadAsset(RecordId soundId)
    {
        SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(SrscRecordType::SOUND, soundId);
        if(dirIt == getSrscFile().getDirectoryEnd())
        {
            Logger::error() << "Sound " << std::hex << soundId << std::dec << " not found in database " << getDatabase().getShortName();
            throw NotFoundException("Texture not found in database");
        }

        Logger::debug() << "Loading sound " << std::hex << soundId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

        osg::ref_ptr<Sound> sound(new Sound(getDatabase(), soundId));
        DataReader dr(getSrscFile().getStreamForRecord(dirIt));
        sound->loadFromRecord(dr);

        return sound;
    }

}




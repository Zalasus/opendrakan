/*
 * AnimationFactory.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include "db/AnimationFactory.h"

#include "SrscRecordTypes.h"
#include "Exception.h"
#include "db/Database.h"
#include "StringUtils.h"

namespace od
{

	AnimationFactory::AnimationFactory(const FilePath &adbFilePath, Database &database)
	: AssetFactory<Animation>(adbFilePath, database)
	{
	}

	osg::ref_ptr<Animation> AnimationFactory::loadAsset(RecordId animId)
	{
		SrscFile::DirIterator infoRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_INFO, animId);
        if(infoRecord == getSrscFile().getDirectoryEnd())
        {
        	Logger::error() << "Animation " << std::hex << animId << std::dec << " not found in database " << getDatabase().getShortName();
            throw NotFoundException("Given animation not found in database");
        }

		Logger::verbose() << "Loading animation " << std::hex << animId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

        osg::ref_ptr<Animation> newAnim(new Animation(getDatabase(), animId));
        newAnim->loadInfo(DataReader(getSrscFile().getStreamForRecord(infoRecord)));

        SrscFile::DirIterator animFramesRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_FRAMES, animId, infoRecord);
        newAnim->loadFrames(DataReader(getSrscFile().getStreamForRecord(animFramesRecord)));

        SrscFile::DirIterator animLookupRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_LOOKUP, animId, infoRecord);
        newAnim->loadFrameLookup(DataReader(getSrscFile().getStreamForRecord(animLookupRecord)));

        return newAnim;
	}

}

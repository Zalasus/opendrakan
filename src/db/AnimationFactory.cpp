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

	AnimationFactory::AnimationFactory(AssetProvider &ap, SrscFile &animationContainer)
	: AssetFactory<Animation>(ap, animationContainer)
	{
	}

	osg::ref_ptr<Animation> AnimationFactory::loadAsset(RecordId animId)
	{
		SrscFile::DirIterator infoRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_INFO, animId);
        if(infoRecord == getSrscFile().getDirectoryEnd())
        {
        	return nullptr;
        }

        osg::ref_ptr<Animation> newAnim(new Animation(getAssetProvider(), animId));

        newAnim->loadInfo(DataReader(getSrscFile().getStreamForRecord(infoRecord)));

        SrscFile::DirIterator animFramesRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_FRAMES, animId, infoRecord);
        newAnim->loadFrames(DataReader(getSrscFile().getStreamForRecord(animFramesRecord)));

        SrscFile::DirIterator animLookupRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::ANIMATION_LOOKUP, animId, infoRecord);
        newAnim->loadFrameLookup(DataReader(getSrscFile().getStreamForRecord(animLookupRecord)));

        return newAnim;
	}

}

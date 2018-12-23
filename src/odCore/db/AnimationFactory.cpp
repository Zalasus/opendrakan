/*
 * AnimationFactory.cpp
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#include <odCore/db/AnimationFactory.h>

#include <odCore/SrscRecordTypes.h>

namespace odDb
{

	AnimationFactory::AnimationFactory(AssetProvider &ap, od::SrscFile &animationContainer)
	: AssetFactory<Animation>(ap, animationContainer)
	{
	}

	od::RefPtr<Animation> AnimationFactory::loadAsset(od::RecordId animId)
	{
		od::SrscFile::DirIterator infoRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::ANIMATION_INFO, animId);
        if(infoRecord == getSrscFile().getDirectoryEnd())
        {
        	return nullptr;
        }

        od::RefPtr<Animation> newAnim = od::make_refd<Animation>(getAssetProvider(), animId);

        newAnim->loadInfo(od::DataReader(getSrscFile().getStreamForRecord(infoRecord)));

        od::SrscFile::DirIterator animFramesRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::ANIMATION_FRAMES, animId, infoRecord);
        newAnim->loadFrames(od::DataReader(getSrscFile().getStreamForRecord(animFramesRecord)));

        od::SrscFile::DirIterator animLookupRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::ANIMATION_LOOKUP, animId, infoRecord);
        newAnim->loadFrameLookup(od::DataReader(getSrscFile().getStreamForRecord(animLookupRecord)));

        return newAnim;
	}

}

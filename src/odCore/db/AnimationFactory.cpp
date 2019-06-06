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
	    auto inputCursor = getSrscFile().getFirstRecordOfTypeId(od::SrscRecordType::ANIMATION_INFO, animId);
        if(!inputCursor.isValid())
        {
        	return nullptr;
        }

        auto infoRecordIt = inputCursor.getDirIterator();

        od::RefPtr<Animation> newAnim = od::make_refd<Animation>(getAssetProvider(), animId);

        newAnim->loadInfo(inputCursor.getReader());

        if(!inputCursor.nextOfTypeId(od::SrscRecordType::ANIMATION_FRAMES, animId, 2))
        {
            throw od::Exception("Found no frames record after animation info record");
        }
        newAnim->loadFrames(inputCursor.getReader());

        inputCursor.moveTo(infoRecordIt);
        if(!inputCursor.nextOfTypeId(od::SrscRecordType::ANIMATION_LOOKUP, animId, 2))
        {
            throw od::Exception("Found no lookup record after animation info record");
        }
        newAnim->loadFrameLookup(inputCursor.getReader());

        return newAnim;
	}

}

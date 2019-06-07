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

        od::RefPtr<Animation> newAnim = od::make_refd<Animation>(getAssetProvider(), animId);
        newAnim->load(std::move(inputCursor));
        return newAnim;
	}

}

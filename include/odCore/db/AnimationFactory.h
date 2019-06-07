/*
 * AnimationFactory.h
 *
 *  Created on: 30 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_ANIMATIONFACTORY_H_
#define INCLUDE_DB_ANIMATIONFACTORY_H_

#include <odCore/db/Animation.h>
#include <odCore/db/AssetFactory.h>

namespace odDb
{

    typedef GenericAssetFactory<Animation> AnimationFactory;

}

#endif /* INCLUDE_DB_ANIMATIONFACTORY_H_ */

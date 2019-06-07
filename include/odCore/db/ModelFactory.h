/*
 * ModelFactory.h
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODELFACTORY_H_
#define INCLUDE_MODELFACTORY_H_

#include <odCore/db/Model.h>
#include <odCore/db/AssetFactory.h>

namespace odDb
{

	typedef GenericAssetFactory<Model> ModelFactory;

}

#endif /* INCLUDE_MODELFACTORY_H_ */

/*
 * SequenceFactory.h
 *
 *  Created on: 2 Mar 2018
 *      Author: zal
 */

#ifndef SEQUENCEFACTORY_H_
#define SEQUENCEFACTORY_H_

#include <odCore/db/AssetFactory.h>
#include <odCore/db/Sequence.h>

namespace odDb
{

    typedef GenericAssetFactory<Sequence> SequenceFactory;

}


#endif /* SEQUENCEFACTORY_H_ */

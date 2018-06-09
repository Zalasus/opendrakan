/*
 * SequenceFactory.h
 *
 *  Created on: 2 Mar 2018
 *      Author: zal
 */

#ifndef SEQUENCEFACTORY_H_
#define SEQUENCEFACTORY_H_

#include "db/AssetFactory.h"
#include "db/Sequence.h"

namespace od
{

    class SequenceFactory : public AssetFactory<Sequence>
    {
    public:

        SequenceFactory(AssetProvider &ap, SrscFile &sequenceContainer);


    protected:

        // implement AssetFactory<Sequence>
        virtual osg::ref_ptr<Sequence> loadAsset(RecordId textureId) override;

    };

}


#endif /* SEQUENCEFACTORY_H_ */

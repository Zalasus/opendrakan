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

    class SequenceFactory : public AssetFactory<Sequence>
    {
    public:

        SequenceFactory(AssetProvider &ap, od::SrscFile &sequenceContainer);


    protected:

        // implement AssetFactory<Sequence>
        virtual AssetPtr<Sequence> loadAsset(od::RecordId textureId) override;

    };

}


#endif /* SEQUENCEFACTORY_H_ */

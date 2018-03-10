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

        SequenceFactory(const FilePath &sdbFilePath, Database &database);


    protected:

        // implement AssetFactory<Sequence>
        SequencePtr loadAsset(RecordId textureId) override;

    };

}


#endif /* SEQUENCEFACTORY_H_ */

/*
 * ClassFactory.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECTTEMPLATEFACTORY_H_
#define OBJECTTEMPLATEFACTORY_H_

#include "ObjectTemplate.h"
#include "AssetFactory.h"

namespace od
{

    class ObjectTemplateFactory : public AssetFactory<ObjectTemplate>
    {
    public:

        ObjectTemplateFactory(const FilePath &odbFilePath, Database &database);


    protected:

        // implement AssetFactory<ObjectTemplate>
        ObjectTemplatePtr loadAsset(RecordId classId);


    private:

        void _loadRflRecord();

    };

}

#endif /* OBJECTTEMPLATEFACTORY_H_ */

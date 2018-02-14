/*
 * ClassFactory.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CLASSFACTORY_H_
#define INCLUDE_CLASSFACTORY_H_

#include "Class.h"
#include "AssetFactory.h"

namespace od
{

    class ClassFactory : public AssetFactory<Class>
    {
    public:

        ClassFactory(const FilePath &odbFilePath, Database &database);


    protected:

        // implement AssetFactory<ClassFactory>
        virtual ClassPtr loadAsset(RecordId classId) override;


    private:

        void _loadRflRecord();

    };

}



#endif /* INCLUDE_CLASSFACTORY_H_ */

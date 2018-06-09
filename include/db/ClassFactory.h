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

    class Engine;

    class ClassFactory : public AssetFactory<Class>
    {
    public:

        ClassFactory(AssetProvider &ap, SrscFile &classContainer);


    protected:

        // implement AssetFactory<ClassFactory>
        virtual osg::ref_ptr<Class> loadAsset(RecordId classId) override;


    private:

        void _loadRflRecord();

    };

}



#endif /* INCLUDE_CLASSFACTORY_H_ */

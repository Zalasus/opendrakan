/*
 * ClassFactory.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CLASSFACTORY_H_
#define INCLUDE_CLASSFACTORY_H_

#include <odCore/db/Class.h>
#include <odCore/db/AssetFactory.h>

namespace od
{

    class Engine;

    class ClassFactory : public AssetFactory<Class>
    {
    public:

        ClassFactory(AssetProvider &ap, SrscFile &classContainer);

        /**
         * @brief Finds the first class record in this factory's container with the given RFL record type.
         *
         * This method is useful for classes that influence gameplay, but are never placed in a level as an
         * object (e.g. Level Properties and User Interface Properties).
         *
         * @return The record ID of the found class, or AssetRef::NULL_REF.assetId if not found
         */
        RecordId findFirstClassOfType(uint16_t rflClassType);


    protected:

        // implement AssetFactory<ClassFactory>
        virtual osg::ref_ptr<Class> loadAsset(RecordId classId) override;


    private:

        void _loadRflRecord();

    };

}



#endif /* INCLUDE_CLASSFACTORY_H_ */

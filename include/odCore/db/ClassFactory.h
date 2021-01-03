/*
 * ClassFactory.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CLASSFACTORY_H_
#define INCLUDE_CLASSFACTORY_H_

#include <odCore/FilePath.h>

#include <odCore/db/Class.h>
#include <odCore/db/AssetFactory.h>

namespace odDb
{

    class ClassFactory : public AssetFactory<Class>
    {
    public:

        ClassFactory(std::shared_ptr<DependencyTable> depTable, od::SrscFile &classContainer);

        inline const od::FilePath &getRflPath() { return mRflPath; }

        /**
         * @brief Finds the first class record in this factory's container with the given RFL record type.
         *
         * This method is useful for classes that influence gameplay, but are never placed in a level as an
         * object (e.g. Level Properties and User Interface Properties).
         *
         * @return The record ID of the found class, or AssetRef::NULL_REF.assetId if not found
         */
        od::RecordId findFirstClassOfType(uint16_t rflClassType);


    protected:

        // implement AssetFactory<Class>
        virtual std::shared_ptr<Class> createNewAsset(od::RecordId id) override;


    private:

        void _loadRflRecord();

        od::FilePath mRflPath;
    };

}



#endif /* INCLUDE_CLASSFACTORY_H_ */

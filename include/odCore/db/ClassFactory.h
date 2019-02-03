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
}

namespace odRfl
{
    class Rfl;
    class RflManager;
}

namespace odDb
{

    class ClassFactory : public AssetFactory<Class>
    {
    public:

        ClassFactory(AssetProvider &ap, od::SrscFile &classContainer, od::Engine &engine);

        inline odRfl::Rfl *getRfl() { return mRfl; }

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

        // implement AssetFactory<ClassFactory>
        virtual od::RefPtr<Class> loadAsset(od::RecordId classId) override;


    private:

        void _loadRflRecord();

        odRfl::RflManager &mRflManager;
        odRfl::Rfl *mRfl;

    };

}



#endif /* INCLUDE_CLASSFACTORY_H_ */

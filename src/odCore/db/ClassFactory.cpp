/*
 * ClassFactory.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include <odCore/db/ClassFactory.h>

#include <odCore/SrscRecordTypes.h>
#include <odCore/Exception.h>
#include <odCore/StringUtils.h>

#include <odCore/db/Database.h>

#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/RflManager.h>

namespace odDb
{

	ClassFactory::ClassFactory(AssetProvider &ap, od::SrscFile &classContainer)
    : AssetFactory<Class>(ap, classContainer)
    {
        _loadRflRecord();
    }

	od::RecordId ClassFactory::findFirstClassOfType(uint16_t rflClassType)
    {
	    auto cursor = getSrscFile().getFirstRecordOfType(od::SrscRecordType::CLASS);
	    while(cursor.isValid())
	    {
	        od::DataReader dr = cursor.getReader();

	        std::string classname;
	        uint16_t type;
	        dr >> classname // this string has variable length, so load it even though we don't need it
	           >> od::DataReader::Ignore(6)
	           >> type;

	        if(type == rflClassType)
	        {
	            return cursor.getDirIterator()->recordId;
	        }

	        cursor.nextOfType(od::SrscRecordType::CLASS);
	    }

	    return AssetRef::NULL_REF.assetId;
    }

    od::RefPtr<Class> ClassFactory::createNewAsset(od::RecordId id)
    {
        return od::make_refd<Class>(getAssetProvider(), id, *this);
    }

    void ClassFactory::_loadRflRecord()
    {
        auto cursor = getSrscFile().getFirstRecordOfType(od::SrscRecordType::CLASS_RFL);
        if(!cursor.isValid())
        {
            throw od::Exception("Class database contained no RFL definition record");
        }

        od::DataReader dr = cursor.getReader();

        std::string rflPathStr;
        dr >> od::DataReader::Ignore(8)
           >> rflPathStr;

        mRflPath = od::FilePath(rflPathStr, getSrscFile().getFilePath().dir());
    }

}

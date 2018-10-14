/*
 * ClassFactory.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include <odCore/db/ClassFactory.h>

#include <odCore/Engine.h>
#include <odCore/SrscRecordTypes.h>
#include <odCore/Exception.h>
#include <odCore/StringUtils.h>
#include <odCore/db/Database.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/rfl/RflManager.h>

namespace odDb
{

	ClassFactory::ClassFactory(AssetProvider &ap, od::SrscFile &classContainer, od::Engine &engine)
    : AssetFactory<Class>(ap, classContainer)
    , mRflManager(engine.getRflManager())
    {
        _loadRflRecord();
    }

	od::RecordId ClassFactory::findFirstClassOfType(uint16_t rflClassType)
    {
	    auto it = this->getSrscFile().getDirIteratorByType(od::SrscRecordType::CLASS);
	    while(it != this->getSrscFile().getDirectoryEnd())
	    {
	        od::DataReader dr(this->getSrscFile().getStreamForRecord(it));

	        std::string classname;
	        uint16_t type;
	        dr >> classname
	           >> od::DataReader::Ignore(6)
	           >> type;

	        if(type == rflClassType)
	        {
	            return it->recordId;
	        }

	        it = this->getSrscFile().getDirIteratorByType(od::SrscRecordType::CLASS, it+1);
	    }

	    return AssetRef::NULL_REF.assetId;
    }

    osg::ref_ptr<Class> ClassFactory::loadAsset(od::RecordId classId)
    {
        od::SrscFile::DirIterator it = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::CLASS, classId);
        if(it == getSrscFile().getDirectoryEnd())
        {
        	return nullptr;
        }

        osg::ref_ptr<Class> newClass(new Class(getAssetProvider(), classId));
        newClass->loadFromRecord(*this, od::DataReader(getSrscFile().getStreamForRecord(it)));

        return newClass;
    }

    void ClassFactory::_loadRflRecord()
    {
        od::SrscFile::DirIterator it = getSrscFile().getDirIteratorByType(od::SrscRecordType::CLASS_RFL);
        if(it == getSrscFile().getDirectoryEnd())
        {
            throw od::Exception("Class database contained no RFL definition record");
        }

        od::DataReader dr(getSrscFile().getStreamForRecord(it));

        std::string rflPathStr;
        dr >> od::DataReader::Ignore(8)
           >> rflPathStr;

        od::FilePath rflPath(rflPathStr, getSrscFile().getFilePath().dir());

        // ignore path part. should we ever encouter multiple RFLs with the same name in different
        //  directories, we need to handle this differently
        mRfl = mRflManager.getRfl(rflPath.fileStrNoExt());
    }

}


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

namespace od
{

	ClassFactory::ClassFactory(AssetProvider &ap, SrscFile &classContainer, Engine &engine)
    : AssetFactory<Class>(ap, classContainer)
    , mRflManager(engine.getRflManager())
    {
        _loadRflRecord();
    }

	RecordId ClassFactory::findFirstClassOfType(uint16_t rflClassType)
    {
	    auto it = this->getSrscFile().getDirIteratorByType(SrscRecordType::CLASS);
	    while(it != this->getSrscFile().getDirectoryEnd())
	    {
	        DataReader dr(this->getSrscFile().getStreamForRecord(it));

	        std::string classname;
	        uint16_t type;
	        dr >> classname
	           >> DataReader::Ignore(6)
	           >> type;

	        if(type == rflClassType)
	        {
	            return it->recordId;
	        }

	        it = this->getSrscFile().getDirIteratorByType(SrscRecordType::CLASS, it+1);
	    }

	    return AssetRef::NULL_REF.assetId;
    }

    osg::ref_ptr<Class> ClassFactory::loadAsset(RecordId classId)
    {
        SrscFile::DirIterator it = getSrscFile().getDirIteratorByTypeId(SrscRecordType::CLASS, classId);
        if(it == getSrscFile().getDirectoryEnd())
        {
        	return nullptr;
        }

        osg::ref_ptr<Class> newClass(new Class(getAssetProvider(), classId));
        newClass->loadFromRecord(*this, DataReader(getSrscFile().getStreamForRecord(it)));

        return newClass;
    }

    void ClassFactory::_loadRflRecord()
    {
        SrscFile::DirIterator it = getSrscFile().getDirIteratorByType(SrscRecordType::CLASS_RFL);
        if(it == getSrscFile().getDirectoryEnd())
        {
            throw Exception("Class database contained no RFL definition record");
        }

        DataReader dr(getSrscFile().getStreamForRecord(it));

        std::string rflPathStr;
        dr >> DataReader::Ignore(8)
           >> rflPathStr;

        FilePath rflPath(rflPathStr, getSrscFile().getFilePath().dir());

        // ignore path part. should we ever encouter multiple RFLs with the same name in different
        //  directories, we need to handle this differently
        mRfl = mRflManager.getRfl(rflPath.fileStrNoExt());
    }

}


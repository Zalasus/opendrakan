/*
 * ClassFactory.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#include "db/ClassFactory.h"

#include "SrscRecordTypes.h"
#include "Exception.h"
#include "db/Database.h"
#include "StringUtils.h"
#include "rfl/Rfl.h"

namespace od
{

	ClassFactory::ClassFactory(AssetProvider &ap, SrscFile &classContainer)
    : AssetFactory<Class>(ap, classContainer)
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

        if(!StringUtils::compareIgnoringCase(rflPath.fileStrNoExt(), odRfl::Rfl::getSingleton().getName()))
        {
            throw UnsupportedException("Class database uses different RFL than linked into engine");
        }
    }

}


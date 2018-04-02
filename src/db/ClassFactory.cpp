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

	ClassFactory::ClassFactory(const FilePath &odbFilePath, Database &database)
    : AssetFactory<Class>(odbFilePath, database)
    {
        _loadRflRecord();
    }

    ClassPtr ClassFactory::loadAsset(RecordId classId)
    {
        SrscFile::DirIterator it = getSrscFile().getDirIteratorByTypeId(SrscRecordType::CLASS, classId);
        if(it == getSrscFile().getDirectoryEnd())
        {
        	Logger::error() << "Class " << std::hex << classId << std::dec << " not found in database " << getDatabase().getShortName();
            throw NotFoundException("Given class not found in database");
        }

		Logger::verbose() << "Loading class " << std::hex << classId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

        ClassPtr newClass(new Class(getDatabase(), classId));
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

        FilePath rflPath(rflPathStr, getDatabase().getDbFilePath().dir());

        if(!StringUtils::compareIgnoringCase(rflPath.fileStrNoExt(), odRfl::Rfl::getSingleton().getName()))
        {
            throw UnsupportedException("Class database uses different RFL than linked into engine");
        }
    }

}


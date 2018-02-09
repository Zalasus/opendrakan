/*
 * ClassFactory.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "SrscRecordTypes.h"
#include "Exception.h"
#include "Database.h"
#include "StringUtils.h"
#include "ObjectTemplateFactory.h"
#include "rfl/RiotFunctionLibrary.h"

namespace od
{

    ObjectTemplateFactory::ObjectTemplateFactory(const FilePath &odbFilePath, Database &database)
    : AssetFactory<ObjectTemplate>(odbFilePath, database)
    {
        _loadRflRecord();
    }

    ObjectTemplatePtr ObjectTemplateFactory::loadAsset(RecordId classId)
    {
        SrscFile::DirIterator it = getSrscFile().getDirIteratorByTypeId(OD_SRSC_CLASS, classId);
        if(it == getSrscFile().getDirectoryEnd())
        {
            throw NotFoundException("Given class not found in database");
        }

		Logger::verbose() << "Loading class " << std::hex << classId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

        ObjectTemplatePtr objectTemplate(new ObjectTemplate(getDatabase(), classId));
        objectTemplate->loadFromRecord(*this, DataReader(getSrscFile().getStreamForRecord(it)));

        return objectTemplate;
    }

    void ObjectTemplateFactory::_loadRflRecord()
    {
        SrscFile::DirIterator it = getSrscFile().getDirIteratorByType(OD_SRSC_CLASS_RFL);
        if(it == getSrscFile().getDirectoryEnd())
        {
            throw Exception("Class database contained no RFL definition record");
        }

        DataReader dr(getSrscFile().getStreamForRecord(it));

        std::string rflPathStr;
        dr >> DataReader::Ignore(8)
           >> rflPathStr;

        FilePath rflPath(rflPathStr, getDatabase().getDbFilePath().dir());

        if(!StringUtils::compareIgnoringCase(rflPath.fileStrNoExt(), RiotFunctionLibrary::getSingleton().getName()))
        {
            throw UnsupportedException("Class database uses different RFL than linked into engine");
        }
    }

}





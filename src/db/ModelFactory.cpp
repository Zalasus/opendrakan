/*
 * ModelFactory.cpp
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#include "db/ModelFactory.h"

#include "Exception.h"
#include "SrscRecordTypes.h"
#include "db/Database.h"

namespace od
{

	ModelFactory::ModelFactory(const FilePath &modFilePath, Database &database)
	: AssetFactory<Model>(modFilePath, database)
	{
	}

	ModelPtr ModelFactory::loadAsset(RecordId id)
	{
		SrscFile::DirIterator nameRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_NAME, id);
		if(nameRecord == getSrscFile().getDirectoryEnd())
		{
			Logger::error() << "Model " << std::hex << id << std::dec << " not found in database " << getDatabase().getShortName();
			throw NotFoundException("Given model not found in database");
		}

		Logger::verbose() << "Loading model " << std::hex << id << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

		// required records
		ModelPtr model(new Model(getDatabase(), id));
		model->loadNameAndShading(*this, DataReader(getSrscFile().getStreamForRecord(nameRecord)));

		SrscFile::DirIterator vertRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_VERTICES, id, nameRecord);
		model->loadVertices(*this, DataReader(getSrscFile().getStreamForRecord(vertRecord)));

		SrscFile::DirIterator texRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_TEXTURES, id, nameRecord);
		model->loadTextures(*this, DataReader(getSrscFile().getStreamForRecord(texRecord)));

		SrscFile::DirIterator faceRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_FACES, id, nameRecord);
		model->loadFaces(*this, DataReader(getSrscFile().getStreamForRecord(faceRecord)));


		// optional records TODO: this regularly causes us to search the whole directory. maybe we should use the order of the records?
		SrscFile::DirIterator lodRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_LOD_BONES, id, nameRecord);
		if(lodRecord != getSrscFile().getDirectoryEnd())
		{
			model->loadLodsAndBones(*this, DataReader(getSrscFile().getStreamForRecord(lodRecord)));
		}

		model->buildGeometry();

		return model;
	}


}

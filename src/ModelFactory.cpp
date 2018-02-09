/*
 * ModelFactory.cpp
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#include "ModelFactory.h"

#include "Exception.h"
#include "SrscRecordTypes.h"
#include "Database.h"

namespace od
{

	ModelFactory::ModelFactory(const FilePath &modFilePath, Database &database)
	: AssetFactory<Model>(modFilePath, database)
	{
	}

	ModelPtr ModelFactory::loadAsset(RecordId id)
	{
		SrscFile::DirIterator nameRecord = getSrscFile().getDirIteratorByTypeId(OD_SRSC_MODEL_NAME, id);
		if(nameRecord == getSrscFile().getDirectoryEnd())
		{
			throw NotFoundException("Given model not found in database");
		}

		Logger::verbose() << "Loading model " << std::hex << id << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

		ModelPtr model(new Model(getDatabase(), id));
		model->loadNameAndShading(*this, DataReader(getSrscFile().getStreamForRecord(nameRecord)));

		SrscFile::DirIterator vertRecord = getSrscFile().getDirIteratorByTypeId(OD_SRSC_MODEL_VERTICES, id, nameRecord);
		model->loadVertices(*this, DataReader(getSrscFile().getStreamForRecord(vertRecord)));

		SrscFile::DirIterator faceRecord = getSrscFile().getDirIteratorByTypeId(OD_SRSC_MODEL_FACES, id, nameRecord);
		model->loadFaces(*this, DataReader(getSrscFile().getStreamForRecord(faceRecord)));

		SrscFile::DirIterator texRecord = getSrscFile().getDirIteratorByTypeId(OD_SRSC_MODEL_TEXTURES, id, nameRecord);
		model->loadTextures(*this, DataReader(getSrscFile().getStreamForRecord(texRecord)));

		model->buildGeometry();

		return model;
	}


}

/*
 * ModelFactory.cpp
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#include <odCore/db/ModelFactory.h>

#include <odCore/Exception.h>
#include <odCore/SrscRecordTypes.h>

namespace odDb
{

	ModelFactory::ModelFactory(AssetProvider &ap, od::SrscFile &modelContainer)
	: AssetFactory<Model>(ap, modelContainer)
	{
	}

	AssetPtr<Model> ModelFactory::loadAsset(od::RecordId id)
	{
		od::SrscFile::DirIterator nameRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_NAME, id);
		if(nameRecord == getSrscFile().getDirectoryEnd())
		{
			return nullptr;
		}

		// required records
		AssetPtr<Model> model(new Model(getAssetProvider(), id));
		model->loadNameAndShading(*this, od::DataReader(getSrscFile().getStreamForRecord(nameRecord)));

		od::SrscFile::DirIterator vertRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_VERTICES, id, nameRecord);
		model->loadVertices(*this, od::DataReader(getSrscFile().getStreamForRecord(vertRecord)));

		od::SrscFile::DirIterator texRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_TEXTURES, id, nameRecord);
		model->loadTextures(*this, od::DataReader(getSrscFile().getStreamForRecord(texRecord)));

		od::SrscFile::DirIterator faceRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_POLYGONS, id, nameRecord);
		model->loadPolygons(*this, od::DataReader(getSrscFile().getStreamForRecord(faceRecord)));

		// optional records FIXME: this regularly causes us to search the whole directory. maybe we should use the order of the records?
		od::SrscFile::DirIterator lodRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_LOD_BONES, id, nameRecord);
		if(lodRecord != getSrscFile().getDirectoryEnd())
		{
			model->loadLodsAndBones(*this, od::DataReader(getSrscFile().getStreamForRecord(lodRecord)));
		}

		od::SrscFile::DirIterator boundingRecord = getSrscFile().getDirIteratorByTypeId(od::SrscRecordType::MODEL_BOUNDING, id, nameRecord);
		if(boundingRecord != getSrscFile().getDirectoryEnd())
		{
			model->loadBoundingData(*this, od::DataReader(getSrscFile().getStreamForRecord(boundingRecord)));
		}

		return model;
	}

}

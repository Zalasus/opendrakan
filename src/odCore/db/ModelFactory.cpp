/*
 * ModelFactory.cpp
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#include <odCore/db/ModelFactory.h>

#include <odCore/Exception.h>
#include <odCore/SrscRecordTypes.h>

namespace od
{

	ModelFactory::ModelFactory(AssetProvider &ap, SrscFile &modelContainer)
	: AssetFactory<Model>(ap, modelContainer)
	{
	}

	osg::ref_ptr<Model> ModelFactory::loadAsset(RecordId id)
	{
		SrscFile::DirIterator nameRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_NAME, id);
		if(nameRecord == getSrscFile().getDirectoryEnd())
		{
			return nullptr;
		}

		// required records
		osg::ref_ptr<Model> model(new Model(getAssetProvider(), id));
		model->loadNameAndShading(*this, DataReader(getSrscFile().getStreamForRecord(nameRecord)));

		SrscFile::DirIterator vertRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_VERTICES, id, nameRecord);
		model->loadVertices(*this, DataReader(getSrscFile().getStreamForRecord(vertRecord)));

		SrscFile::DirIterator texRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_TEXTURES, id, nameRecord);
		model->loadTextures(*this, DataReader(getSrscFile().getStreamForRecord(texRecord)));

		SrscFile::DirIterator faceRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_POLYGONS, id, nameRecord);
		model->loadPolygons(*this, DataReader(getSrscFile().getStreamForRecord(faceRecord)));

		// optional records FIXME: this regularly causes us to search the whole directory. maybe we should use the order of the records?
		SrscFile::DirIterator lodRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_LOD_BONES, id, nameRecord);
		if(lodRecord != getSrscFile().getDirectoryEnd())
		{
			model->loadLodsAndBones(*this, DataReader(getSrscFile().getStreamForRecord(lodRecord)));
		}

		SrscFile::DirIterator boundingRecord = getSrscFile().getDirIteratorByTypeId(SrscRecordType::MODEL_BOUNDING, id, nameRecord);
		if(boundingRecord != getSrscFile().getDirectoryEnd())
		{
			model->loadBoundingData(*this, DataReader(getSrscFile().getStreamForRecord(boundingRecord)));
		}

		return model;
	}

}

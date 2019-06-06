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

	od::RefPtr<Model> ModelFactory::loadAsset(od::RecordId id)
	{
		auto cursor = getSrscFile().getFirstRecordOfTypeId(od::SrscRecordType::MODEL_NAME, id);
		if(!cursor.isValid())
		{
			return nullptr;
		}

		auto nameRecordIt = cursor.getDirIterator();

		// required records
		od::RefPtr<Model> model = od::make_refd<Model>(getAssetProvider(), id);
		model->loadNameAndShading(*this, cursor.getReader());

		if(!cursor.nextOfTypeId(od::SrscRecordType::MODEL_VERTICES, id, 8))
		{
		    throw od::Exception("Found no vertex record after model name record");
		}
		model->loadVertices(*this, cursor.getReader());

		cursor.moveTo(nameRecordIt);
		if(!cursor.nextOfTypeId(od::SrscRecordType::MODEL_TEXTURES, id, 8))
        {
            throw od::Exception("Found no texture record after model name record");
        }
		model->loadTextures(*this, cursor.getReader());

		cursor.moveTo(nameRecordIt);
        if(!cursor.nextOfTypeId(od::SrscRecordType::MODEL_POLYGONS, id, 8))
        {
            throw od::Exception("Found no polyon record after model name record");
        }
		model->loadPolygons(*this, cursor.getReader());

		// optional records
		cursor.moveTo(nameRecordIt);
        if(cursor.nextOfTypeId(od::SrscRecordType::MODEL_LOD_BONES, id, 8))
        {
            model->loadLodsAndBones(*this, cursor.getReader());
        }

		cursor.moveTo(nameRecordIt);
        if(cursor.nextOfTypeId(od::SrscRecordType::MODEL_BOUNDING, id, 8))
        {
			model->loadBoundingData(*this, cursor.getReader());
		}

		return model;
	}

}

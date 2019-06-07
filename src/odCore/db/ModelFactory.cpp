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

		od::RefPtr<Model> model = od::make_refd<Model>(getAssetProvider(), id, *this);
		model->load(std::move(cursor));
		return model;
	}

}

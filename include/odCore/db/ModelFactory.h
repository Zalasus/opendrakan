/*
 * ModelFactory.h
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODELFACTORY_H_
#define INCLUDE_MODELFACTORY_H_

#include <odCore/db/Model.h>
#include <odCore/db/AssetFactory.h>
#include <odCore/SrscFile.h>

namespace od
{

	class ModelFactory : public AssetFactory<Model>
	{
	public:

		ModelFactory(AssetProvider &ap, SrscFile &modelContainer);


	protected:

		// implement AssetFactory<Model>
		virtual osg::ref_ptr<Model> loadAsset(RecordId id) override;

	};

}

#endif /* INCLUDE_MODELFACTORY_H_ */

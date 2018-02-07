/*
 * ModelFactory.h
 *
 *  Created on: 7 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODELFACTORY_H_
#define INCLUDE_MODELFACTORY_H_

#include "AssetFactory.h"
#include "FilePath.h"
#include "SrscFile.h"
#include "Model.h"

namespace od
{

	class Database;

	class ModelFactory : public AssetFactory<Model>
	{
	public:

		ModelFactory(const FilePath &modFilePath, Database &database);


	protected:

		virtual ModelPtr loadAsset(RecordId id);

	};

}

#endif /* INCLUDE_MODELFACTORY_H_ */

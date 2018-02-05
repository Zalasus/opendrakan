/*
 * Model.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <string>
#include <osg/Group>

#include "Asset.h"

namespace od
{

	class Model : public osg::Group
	{
	public:

		Model();

		void loadFromRecord(SrscFile &srscFile, RecordId id);


	private:

		void _loadNameAndShading(DataReader dr);
		void _loadVertices(DataReader dr);
		void _loadFaces(DataReader dr);
		void _loadMaterials(DataReader dr);
		void _loadBoundingData(DataReader dr);

		std::string mModelName;
	};

}

#endif /* INCLUDE_MODEL_H_ */

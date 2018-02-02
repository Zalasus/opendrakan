/*
 * Model.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <string>

#include "RiotAsset.h"

namespace od
{

	class Model : public RiotAsset
	{
	public:

		Model();

		virtual void loadFromRecord(SrscFile &srscFile, RecordId id);
		virtual std::string getName();


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

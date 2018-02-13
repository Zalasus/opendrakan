/*
 * Model.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <string>
#include <osg/Vec3>
#include <osg/Texture2D>

#include "Asset.h"
#include "SegmentedGeode.h"

namespace od
{

	class ModelFactory;

	class Model : public Asset, public SegmentedGeode
	{
	public:

		Model(Database &db, RecordId modelId);

		void loadNameAndShading(ModelFactory &factory, DataReader dr);
		void loadVertices(ModelFactory &factory, DataReader dr);
		void loadTextures(ModelFactory &factory, DataReader dr);
		void loadFaces(ModelFactory &factory, DataReader dr);
		void loadBoundingData(ModelFactory &factory, DataReader dr);
		void buildGeometry();

		// implement Asset
        virtual const char *getAssetTypeName() const override { return "model"; }

		// override osg::Group
		virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "Model"; }


	private:

		std::string mModelName;
		std::vector<osg::Vec3> mVertices;
		std::vector<Face> mFaces;
		size_t mTriangleCount;
		size_t mQuadCount;
		std::vector<AssetRef> mTextureRefs;

		bool mVerticesLoaded;
		bool mTexturesLoaded;
		bool mFacesLoaded;
	};

	typedef osg::ref_ptr<Model> ModelPtr;
}

#endif /* INCLUDE_MODEL_H_ */

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
#include <osg/Group>

#include "Asset.h"
#include "SegmentedGeode.h"
#include "Skeleton.h"

namespace od
{

	struct LodMeshInfo
	{
		std::string lodName;
		float distanceThreshold;
		uint32_t usage;
		uint32_t nodeIndex;
		uint32_t firstVertexIndex;
		uint32_t vertexCount;
		uint32_t firstFaceIndex;
		uint32_t faceCount;
	};


	class ModelFactory;

	class Model : public Asset, public osg::Group
	{
	public:

	    struct VertexAffection
	    {
	        VertexAffection() : affectingBoneCount(0) {}

	        size_t affectingBoneCount;
	    };

		Model(Database &db, RecordId modelId);

		// TODO: implement copy constructor etc. in a meaningful way for this macro to work
		//META_Node(od, Model);

		void loadNameAndShading(ModelFactory &factory, DataReader &&dr);
		void loadVertices(ModelFactory &factory, DataReader &&dr);
		void loadTextures(ModelFactory &factory, DataReader &&dr);
		void loadFaces(ModelFactory &factory, DataReader &&dr);
		void loadBoundingData(ModelFactory &factory, DataReader &&dr);
		void loadLodsAndBones(ModelFactory &factory, DataReader &&dr);
		void buildGeometry();

		// implement Asset
        virtual const char *getAssetTypeName() const override { return "model"; }


	private:

		std::string mModelName;
		std::vector<osg::Vec3f> mVertices;
		std::vector<VertexAffection> mVertexAffections;
		std::vector<SegmentedGeode::Face> mFaces;
		size_t mTriangleCount;
		size_t mQuadCount;
		std::vector<AssetRef> mTextureRefs;
		std::vector<LodMeshInfo> mLodMeshInfos;
		std::vector<AssetRef> mAnimationRefs;

		bool mVerticesLoaded;
		bool mTexturesLoaded;
		bool mFacesLoaded;
	};

	typedef osg::ref_ptr<Model> ModelPtr;
}

#endif /* INCLUDE_MODEL_H_ */

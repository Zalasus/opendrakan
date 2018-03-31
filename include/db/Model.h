/*
 * Model.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <memory>
#include <string>
#include <osg/Vec3>
#include <osg/Texture2D>
#include <osg/Group>

#include "Asset.h"
#include "Skeleton.h"
#include "GeodeBuilder.h"

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
		uint32_t firstPolygonIndex;
		uint32_t polygonCount;

		std::vector<BoneAffection> boneAffections;
	};


	class ModelFactory;

	class Model : public Asset, public osg::Group
	{
	public:

		Model(Database &db, RecordId modelId);

		// TODO: implement copy constructor etc. in a meaningful way for this macro to work
		//META_Node(od, Model);

		/// Returns SkeletonBuilder that can be used to construct a skeleton for an Object. May return nullptr if no skeleton present.
		inline SkeletonBuilder *getSkeletonBuilder() { return mSkeletonBuilder.get(); }
		inline const std::vector<AssetRef> &getAnimationRefs() { return mAnimationRefs; }

		void loadNameAndShading(ModelFactory &factory, DataReader &&dr);
		void loadVertices(ModelFactory &factory, DataReader &&dr);
		void loadTextures(ModelFactory &factory, DataReader &&dr);
		void loadPolygons(ModelFactory &factory, DataReader &&dr);
		void loadBoundingData(ModelFactory &factory, DataReader &&dr);
		void loadLodsAndBones(ModelFactory &factory, DataReader &&dr);
		void buildGeometry();

		// implement Asset
        virtual const char *getAssetTypeName() const override { return "model"; }


	private:

		std::string mModelName;
		std::vector<osg::Vec3f> mVertices;
		std::vector<AssetRef> mTextureRefs;
		std::vector<Polygon> mPolygons;
		std::vector<LodMeshInfo> mLodMeshInfos;
		std::vector<AssetRef> mAnimationRefs;
		std::unique_ptr<SkeletonBuilder> mSkeletonBuilder;

		bool mVerticesLoaded;
		bool mTexturesLoaded;
		bool mPolygonsLoaded;
	};

	typedef osg::ref_ptr<Model> ModelPtr;
}

#endif /* INCLUDE_MODEL_H_ */

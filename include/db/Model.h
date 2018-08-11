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

#include "physics/ModelBounds.h"
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

	enum class ModelShadingType
	{
	    None,
	    Flat,
	    Smooth
	};

	class ModelFactory;

	class Model : public Asset, public osg::Group
	{
	public:

		Model(AssetProvider &ap, RecordId modelId);
        Model(const Model &model) = delete; // models should never be copied as they can be reused throughout the scenegraph

		/// Returns SkeletonBuilder that can be used to construct a skeleton for an Object. May return nullptr if no skeleton present.
		inline SkeletonBuilder *getSkeletonBuilder() { return mSkeletonBuilder.get(); }
		inline ModelBounds *getModelBounds() { return mModelBounds.get(); }
		inline const std::vector<AssetRef> &getAnimationRefs() { return mAnimationRefs; }
		inline bool isCharacter() const { return mSkeletonBuilder != nullptr; }
		inline bool hasBounds() const { return mModelBounds != nullptr; }
		inline ModelShadingType getShadingType() const { return mShadingType; }

		void loadNameAndShading(ModelFactory &factory, DataReader &&dr);
		void loadVertices(ModelFactory &factory, DataReader &&dr);
		void loadTextures(ModelFactory &factory, DataReader &&dr);
		void loadPolygons(ModelFactory &factory, DataReader &&dr);
		void loadBoundingData(ModelFactory &factory, DataReader &&dr);
		void loadLodsAndBones(ModelFactory &factory, DataReader &&dr);
		void buildGeometry();

		/**
		 * @brief Returns an axis-aligned bounding box that encapsulates all of this model's meshes and LODs.
		 *
		 * This ignores any bounding info stored in the model. The box's expands are directly calculated from the vertex data.
		 */
		inline osg::BoundingBox getCalculatedBoundingBox() { return mCalculatedBoundingBox; }


	private:

		std::string mModelName;
		ModelShadingType mShadingType;
		bool mBlendWithLandscape;
		bool mShiny;
		bool mUseAdditiveBlending;
		bool mEnvironmentMapped;
		std::vector<osg::Vec3f> mVertices;
		std::vector<AssetRef> mTextureRefs;
		std::vector<Polygon> mPolygons;
		std::vector<LodMeshInfo> mLodMeshInfos;
		std::vector<AssetRef> mAnimationRefs;
		std::unique_ptr<ModelBounds> mModelBounds;
		std::unique_ptr<SkeletonBuilder> mSkeletonBuilder;
		bool mVerticesLoaded;
		bool mTexturesLoaded;
		bool mPolygonsLoaded;
		osg::BoundingBox mCalculatedBoundingBox;
	};

	template <>
    struct AssetTraits<Model>
    {
        static const char *name()
        {
            return "Model";
        }
    };

}

#endif /* INCLUDE_MODEL_H_ */

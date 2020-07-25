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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <odCore/BoundingSphere.h>
#include <odCore/BoundingBox.h>

#include <odCore/db/Asset.h>
#include <odCore/db/SkeletonBuilder.h>

namespace odRender
{
    class Model;
    class Renderer;
}

namespace odPhysics
{
    class PhysicsSystem;
    class ModelShape;
}

namespace odDb
{
    class ModelBounds;

	class Model : public Asset
	{
	public:

	    struct BoneAffection
        {
            size_t jointIndex;
            size_t vertexIndex;
            float vertexWeight;
        };

        struct Polygon
        {
            size_t vertexCount;
            size_t vertexIndices[4];
            glm::vec2 uvCoords[4];
            bool doubleSided;
            odDb::AssetRef texture;
        };

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

        enum class ShadingType
        {
            None,
            Flat,
            Smooth
        };

		Model(AssetProvider &ap, od::RecordId modelId);
        Model(const Model &model) = delete; // models should never be copied as they can be reused throughout the scenegraph
        ~Model();

        inline const std::string &getName() const { return mModelName; }
		inline SkeletonBuilder *getSkeletonBuilder() { return mSkeletonBuilder.get(); } ///< May return nullptr if no skeleton present. Can't be const due to some caching in SkeletonBuilder
		inline const std::vector<AssetRef> &getAnimationRefs() { return mAnimationRefs; }
		inline bool hasSkeleton() const { return mSkeletonBuilder != nullptr; }
		inline ShadingType getShadingType() const { return mShadingType; }
		inline bool isShiny() const { return mShiny; }
		inline const std::vector<glm::vec3> &getVertexVector() { return mVertices; }
		inline const std::vector<Polygon> &getPolygonVector() { return mPolygons; }
		inline const std::vector<LodMeshInfo> &getLodInfoVector() { return mLodMeshInfos; }
		inline const od::BoundingSphere &getCalculatedBoundingSphere() const { return mCalculatedBoundingSphere; }
		inline const od::AxisAlignedBoundingBox &getCalculatedBoundingBox() const { return mCalculatedBoundingBox; }

        inline std::weak_ptr<odRender::Model> &getCachedRenderModel() { return mCachedRenderModel; }
        inline std::weak_ptr<odPhysics::ModelShape> &getCachedPhysicsShape() { return mCachedPhysicsShape; }

		const ModelBounds &getModelBounds(size_t lodIndex = 0);

		virtual void load(od::SrscFile::RecordInputCursor cursor) override;


	private:

		void _loadNameAndShading(od::DataReader dr);
        void _loadVertices(od::DataReader dr);
        void _loadTextures(od::DataReader dr);
        void _loadPolygons(od::DataReader dr);
        void _loadBoundingData(od::DataReader dr);
        void _loadLodsAndBones(od::DataReader dr);

		std::string mModelName;
		ShadingType mShadingType;
		bool mBlendWithLandscape;
		bool mShiny;
		bool mUseAdditiveBlending;
		bool mEnvironmentMapped;
		std::vector<glm::vec3> mVertices;
		std::vector<AssetRef> mTextureRefs;
		std::vector<Polygon> mPolygons;
		std::vector<LodMeshInfo> mLodMeshInfos;
		std::vector<AssetRef> mAnimationRefs;
		std::vector<ModelBounds> mModelBounds; // one for each LOD
		std::unique_ptr<SkeletonBuilder> mSkeletonBuilder;
		bool mVerticesLoaded;
		bool mTexturesLoaded;
		bool mPolygonsLoaded;

		od::AxisAlignedBoundingBox mCalculatedBoundingBox;
		od::BoundingSphere mCalculatedBoundingSphere;

		std::weak_ptr<odRender::Model> mCachedRenderModel;
		std::weak_ptr<odPhysics::ModelShape> mCachedPhysicsShape;
	};

	template <>
    struct AssetTraits<Model>
    {
        static const char *name()
        {
            return "Model";
        }

        static constexpr od::RecordType baseType()
        {
            return static_cast<od::RecordType>(od::SrscRecordType::MODEL_NAME);
        }
    };

}

#endif /* INCLUDE_MODEL_H_ */

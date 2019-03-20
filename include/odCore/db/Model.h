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
#include <odCore/WeakRefPtr.h>

#include <odCore/db/Asset.h>
#include <odCore/db/SkeletonBuilder.h>

namespace odRender
{
    class ModelNode;
    class Renderer;
}

namespace odPhysics
{
    class PhysicsSystem;
    class ModelShape;
}

namespace odDb
{
    class ModelFactory;
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
		inline SkeletonBuilder *getSkeletonBuilder() { return mSkeletonBuilder.get(); } ///< May return nullptr if no skeleton present.
		inline const std::vector<AssetRef> &getAnimationRefs() { return mAnimationRefs; }
		inline bool hasSkeleton() const { return mSkeletonBuilder != nullptr; }
		inline ShadingType getShadingType() const { return mShadingType; }
		inline bool isShiny() const { return mShiny; }
		inline const std::vector<glm::vec3> &getVertexVector() { return mVertices; }
		inline const std::vector<Polygon> &getPolygonVector() { return mPolygons; }
		inline const std::vector<LodMeshInfo> &getLodInfoVector() { return mLodMeshInfos; }
		inline const od::BoundingSphere &getCalculatedBoundingSphere() const { return mCalculatedBoundingSphere; }
		inline const od::AxisAlignedBoundingBox &getCalculatedBoundingBox() const { return mCalculatedBoundingBox; }

		ModelBounds &getModelBounds(size_t lodIndex = 0);

		void loadNameAndShading(ModelFactory &factory, od::DataReader &&dr);
		void loadVertices(ModelFactory &factory, od::DataReader &&dr);
		void loadTextures(ModelFactory &factory, od::DataReader &&dr);
		void loadPolygons(ModelFactory &factory, od::DataReader &&dr);
		void loadBoundingData(ModelFactory &factory, od::DataReader &&dr);
		void loadLodsAndBones(ModelFactory &factory, od::DataReader &&dr);

		// returns a refptr since this class only takes weak ownership of potentially created objects
		od::RefPtr<odRender::ModelNode> getOrCreateRenderNode(odRender::Renderer *renderer);

		od::RefPtr<odPhysics::ModelShape> getOrCreateModelShape(odPhysics::PhysicsSystem &ps);


	private:

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

		od::WeakObserverRefPtr<odRender::ModelNode> mRenderNode;

		od::WeakObserverRefPtr<odPhysics::ModelShape> mPhysicsShape;
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

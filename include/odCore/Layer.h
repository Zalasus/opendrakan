/*
 * Layer.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef LAYER_H_
#define LAYER_H_

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <odCore/IdTypes.h>
#include <odCore/DataStream.h>
#include <odCore/BoundingBox.h>
#include <odCore/OdDefines.h>
#include <odCore/LightCallback.h>

#include <odCore/db/Asset.h>

#include <odCore/render/Handle.h>

#define OD_LAYER_FLAG_DIV_BACKSLASH 1

namespace odPhysics
{
    class PhysicsSystem;
    class LayerHandle;
}

namespace odRender
{
    class Renderer;
}

namespace od
{
    class Level;

    class Layer : public LightCallback
    {
    public:

        enum LayerType
        {
            TYPE_FLOOR = 0,
            TYPE_CEILING = 1,
            TYPE_BETWEEN = 2
        };

        enum LightDropoffType
        {
            DROPOFF_NONE = 0,
            DROPOFF_N2S = 1,
            DROPOFF_E2W = 2,
            DROPOFF_S2N = 3,
            DROPOFF_W2E = 4
        };

        struct Cell
        {
            uint16_t flags;
            odDb::AssetRef leftTextureRef;
            odDb::AssetRef rightTextureRef;
            uint16_t texCoords[8];

            inline bool isBackslashCell() { return flags & OD_LAYER_FLAG_DIV_BACKSLASH; }
        };

        struct Vertex
        {
            uint8_t type;
            float heightOffsetLu;
        };

        /// @brief The magic texture reference that indicates a hole in the layer.
        static const odDb::AssetRef HoleTextureRef;

        /// @brief The magic texture reference that indicates an invisible triangle (still with collision)
        static const odDb::AssetRef InvisibleTextureRef;

        Layer(Level &level);
        virtual ~Layer();

        inline Level &getLevel() { return mLevel; }
        inline LayerId getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayerIndices() { return mVisibleLayers; };
        inline uint32_t getOriginX() const { return mOriginX; }
        inline uint32_t getOriginZ() const { return mOriginZ; }
        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }
        inline float getWorldHeightWu() const { return mWorldHeightWu; }
        inline float getWorldHeightLu() const { return OD_WORLD_SCALE * mWorldHeightWu; }
        inline glm::vec3 getLightColor() const { return mLightColor; }
        inline glm::vec3 getAmbientColor() const { return mAmbientColor; }
        inline glm::vec3 getLightDirection() const { return mLightDirectionVector; } ///< Returns direction towards layer light
        inline float getMinHeight() const { return mMinHeight; }
        inline float getMaxHeight() const { return mMaxHeight; }
        inline const AxisAlignedBoundingBox &getBoundingBox() { return mBoundingBox; }
        inline const std::vector<Vertex> &getVertexVector() { return mVertices; }
        inline const std::vector<Cell> &getCellVector() { return mCells; }
        inline size_t getVisibleTriangleCount() const { return mVisibleTriangles; }
        inline size_t getCollidingTriangleCount() const { return mCollidingTriangles; }
        inline LayerType getLayerType() const { return mType; }
        inline bool isSpawned() const { return mIsSpawned; }

        inline glm::vec3 getOrigin() const { return glm::vec3(mOriginX, getWorldHeightLu(), mOriginZ); }

        /// Returns vertex at (x, z) in world space. Caller is responsible to check that x and z are within bounds
        inline glm::vec3 getWorldVertexAt(int32_t x, int32_t z) const
        {
            return getVertexAt(x, z) + getOrigin();
        }

        /// Returns vertex at (x, z) in model space. Caller is responsible to check that x and z are within bounds
        inline glm::vec3 getVertexAt(int32_t x, int32_t z) const
        {
            return glm::vec3(x, mVertices[x + (mWidth+1)*z].heightOffsetLu, z);
        }

        // Returns vertex at vertex index i in model space. Caller is responsible to check that i is within bounds
        inline glm::vec3 getVertexAt(int32_t i) const
        {
            int32_t x = i%(mWidth+1);
            int32_t z = i/(mWidth+1);
            return glm::vec3(x, mVertices[i].heightOffsetLu, z);
        }

        void loadDefinition(DataReader &dr);
        void loadPolyData(DataReader &dr);

        void spawn(odPhysics::PhysicsSystem &physicsSystem, odRender::Renderer *renderer);
        void despawn();

        /**
         * Checks whether this layer has a hole at the absolute xz coordiante \c absolutePos.
         * Coordinates are in lu, relative to the level's origin.
         *
         * If the passed coordinate is outside the layer's bounds, true will be returned.
         */
        bool hasHoleAt(const glm::vec2 &absolutePos);

        bool contains(const glm::vec2 &xzCoord);
        bool contains(const glm::vec2 &xzCoord, float epsilon);

        float getAbsoluteHeightAt(const glm::vec2 &xzCoord);

        virtual void removeAffectingLight(od::Light *light) override;
        virtual void addAffectingLight(od::Light *light) override;
        virtual void clearLightList() override;


    private:

        void _bakeStaticLight(od::Light *light);
        void _bakeLocalLayerLight();

        void _calculateNormalsInternal();

        Level              	   &mLevel;
        LayerId                 mId;
        uint32_t                mWidth;
        uint32_t                mHeight;
        LayerType               mType;
        uint32_t                mOriginX;
        uint32_t                mOriginZ;
        float                   mWorldHeightWu;
        std::string             mLayerName;
        uint32_t                mFlags; // 2 = member of alternate blending group
        float                   mLightDirection;
        float                   mLightAscension;
        glm::vec3               mLightColor;
        glm::vec3               mAmbientColor;
        glm::vec3               mLightDirectionVector; // direction _towards_ light!
        LightDropoffType        mLightDropoffType;
        std::vector<uint32_t>   mVisibleLayers;

        std::vector<Vertex> mVertices;
        std::vector<Cell>   mCells;
        size_t mVisibleTriangles;
        size_t mCollidingTriangles;

        float mMinHeight;
        float mMaxHeight;
        AxisAlignedBoundingBox mBoundingBox;

        std::shared_ptr<odRender::Handle> mRenderHandle;
        std::shared_ptr<odRender::Model> mRenderModel;
        std::shared_ptr<odPhysics::LayerHandle> mPhysicsHandle;

        std::vector<glm::vec3> mLocalNormals; // temporary array, unused right now

        bool mIsSpawned;
    };

}


#endif /* LAYER_H_ */

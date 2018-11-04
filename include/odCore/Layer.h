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
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <odCore/DataStream.h>
#include <odCore/BoundingBox.h>
#include <odCore/OdDefines.h>
#include <odCore/db/Asset.h>
#include <odCore/render/LightState.h>
#include <odCore/render/Renderable.h>

namespace od
{
    class Level;

    class Layer
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

        /// @brief The magic texture reference that indicates a hole in the layer.
        static const odDb::AssetRef HoleTextureRef;

        /// @brief The magic texture reference that indicates an invisible triangle (still with collision)
        static const odDb::AssetRef InvisibleTextureRef;

        Layer(Level &level);
        ~Layer();

        void loadDefinition(DataReader &dr);
        void loadPolyData(DataReader &dr);
        btCollisionShape *getCollisionShape();

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



        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };
        inline uint32_t getOriginX() const { return mOriginX; }
        inline uint32_t getOriginZ() const { return mOriginZ; }
        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }
        inline float getWorldHeightWu() const { return mWorldHeightWu; }
        inline float getWorldHeightLu() const { return OD_WORLD_SCALE * mWorldHeightWu; }
        inline glm::vec3 getLightColor() const { return mLightColor; }
        inline glm::vec3 getAmbientColor() const { return mAmbientColor; }
        inline glm::vec3 getLightDirection() const { return mLightDirectionVector; } ///< Returns direction towards layer light
        inline const AxisAlignedBoundingBox &getBoundingBox() { return mBoundingBox; }


    private:


        struct Cell
        {
            uint16_t flags;
            odDb::AssetRef leftTextureRef;
            odDb::AssetRef rightTextureRef;
            uint16_t texCoords[8];
        };

        struct Vertex
        {
            uint8_t type;
            float heightOffsetLu;
        };

        Level              	   &mLevel;
        uint32_t                mId;
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

        std::unique_ptr<btTriangleMesh> mBulletMesh;
        std::unique_ptr<btCollisionShape> mCollisionShape;
        AxisAlignedBoundingBox mBoundingBox;
    };

}


#endif /* LAYER_H_ */

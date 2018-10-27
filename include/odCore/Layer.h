/*
 * Layer.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef LAYER_H_
#define LAYER_H_

#include <memory>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Light>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <odCore/DataStream.h>
#include <odCore/OdDefines.h>
#include <odCore/db/Asset.h>
#include <odCore/render/LightState.h>

namespace od
{
    class Level;

    class Layer : public osg::PositionAttitudeTransform
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
        void buildGeometry();
        btCollisionShape *getCollisionShape();

        /**
         * Bakes lighting of overlapping layers into this layer's geometry. Call only after all potentially overlapping
         * layer's geometries have been built.
         */
        void bakeOverlappingLayerLighting();

        /**
         * Checks whether this layer has a hole at the absolute xz coordiante \c absolutePos.
         * Coordinates are in lu, relative to the level's origin.
         *
         * If the passed coordinate is outside the layer's bounds, true will be returned.
         */
        bool hasHoleAt(const osg::Vec2 &absolutePos);

        bool contains(const osg::Vec2 &xzCoord);
        bool contains(const osg::Vec2 &xzCoord, float epsilon);

        float getAbsoluteHeightAt(const osg::Vec2 &xzCoord);



        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };
        inline uint32_t getOriginX() const { return mOriginX; }
        inline uint32_t getOriginZ() const { return mOriginZ; }
        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }
        inline float getWorldHeightWu() const { return mWorldHeightWu; }
        inline float getWorldHeightLu() const { return OD_WORLD_SCALE * mWorldHeightWu; }
        inline osg::Vec3 getLightColor() const { return mLightColor; }
        inline osg::Vec3 getAmbientColor() const { return mAmbientColor; }
        inline osg::Vec3 getLightDirection() const { return mLightDirectionVector; } ///< Returns direction towards layer light
        inline const osg::BoundingBox &getBoundingBox() { return mBoundingBox; }

    private:

        void _bakeLocalLayerLight();

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
            osg::Vec3 calculatedLightColor;
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
        osg::Vec3               mLightColor;
        osg::Vec3               mAmbientColor;
        osg::Vec3               mLightDirectionVector; // direction _towards_ light!
        LightDropoffType        mLightDropoffType;
        std::vector<uint32_t>   mVisibleLayers;

        std::vector<Vertex> mVertices;
        std::vector<Cell>   mCells;
        size_t mVisibleTriangles;
        osg::ref_ptr<osg::Geode> mLayerGeode;
        osg::ref_ptr<osg::Vec3Array> mGeometryVertexArray;
        osg::ref_ptr<osg::Vec3Array> mGeometryNormalArray;
        osg::ref_ptr<osg::Vec4Array> mGeometryColorArray;
        osg::ref_ptr<odRender::LightStateCallback> mLightCallback;

        std::unique_ptr<btTriangleMesh> mBulletMesh;
        std::unique_ptr<btCollisionShape> mCollisionShape;

        osg::BoundingBox mBoundingBox;
    };

}


#endif /* LAYER_H_ */

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

#include "db/Asset.h"
#include "DataStream.h"
#include "OdDefines.h"

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

        Layer(Level &level);

        void loadDefinition(DataReader &dr);
        void loadPolyData(DataReader &dr);
        void buildGeometry();
        btCollisionShape *getCollisionShape();

        /**
         * Checks whether this layer has a hole at the absolute xz coordiante \c absolutePos.
         * Coordinates are in lu, relative to the level's origin.
         *
         * If the passed coordinate is outside the layer's bounds, true will be returned.
         */
        bool hasHoleAt(const osg::Vec2 &absolutePos);

        bool contains(const osg::Vec2 &xzCoord);

        float getAbsoluteHeightAt(const osg::Vec2 &xzCoord);

        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };
        inline uint32_t getOriginX() const { return mOriginX; }
        inline uint32_t getOriginZ() const { return mOriginZ; }
        inline float getWorldHeightWu() const { return mWorldHeightWu; }
        inline float getWorldHeightLu() const { return OD_WORLD_SCALE * mWorldHeightWu; }
        inline osg::Light *getLayerLight() const { return mLayerLight; }


    private:

        struct Cell
        {
            uint16_t flags;
            AssetRef leftTextureRef;
            AssetRef rightTextureRef;
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
        osg::Vec4               mLightColor;
        osg::Vec4               mAmbientColor;
        LightDropoffType        mLightDropoffType;
        std::vector<uint32_t>   mVisibleLayers;

        std::vector<Vertex> mVertices;
        std::vector<Cell>   mCells;
        size_t mVisibleTriangles;
        osg::ref_ptr<osg::Geode> mLayerGeode;
        osg::ref_ptr<osg::Light> mLayerLight;

        std::unique_ptr<btTriangleMesh> mBulletMesh;
        std::unique_ptr<btCollisionShape> mCollisionShape;
    };

}


#endif /* LAYER_H_ */

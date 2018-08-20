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

        Layer(Level &level);

        void loadDefinition(DataReader &dr);
        void loadPolyData(DataReader &dr);
        void buildGeometry(osg::Group *layerGroup);
        btCollisionShape *getCollisionShape();

        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };
        inline uint32_t getOriginX() const { return mOriginX; }
        inline uint32_t getOriginZ() const { return mOriginZ; }
        inline float getWorldHeightLu() const { return mWorldHeight * OD_WORLD_SCALE; }


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
            int32_t heightOffset;
            float absoluteHeight;
        };

        Level              	   &mLevel;
        uint32_t                mId;
        uint32_t                mWidth;
        uint32_t                mHeight;
        LayerType               mType;
        uint32_t                mOriginX;
        uint32_t                mOriginZ;
        float                   mWorldHeight;
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

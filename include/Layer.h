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
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include "db/Asset.h"
#include "DataStream.h"

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
        void buildCollisionShape();

        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };


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
        uint32_t                mLightColor;
        uint32_t                mAmbientColor;
        LightDropoffType        mLightDropoffType;
        std::vector<uint32_t>   mVisibleLayers;

        std::vector<Vertex> mVertices;
        std::vector<Cell>   mCells;
        size_t mVisibleTriangles;
        osg::ref_ptr<osg::Geode> mLayerGeode;

        std::unique_ptr<btTriangleMesh> mBulletMesh;
        std::unique_ptr<btBvhTriangleMeshShape> mCollisionShape;
    };

}


#endif /* LAYER_H_ */

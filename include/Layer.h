/*
 * Layer.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef LAYER_H_
#define LAYER_H_

#include "db/Asset.h"
#include "SegmentedGeode.h"
#include "DataStream.h"

namespace od
{
    class Level;

    class Layer : public SegmentedGeode
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

        inline uint32_t getId() const { return mId; };
        inline std::string getName() const { return mLayerName; };
        inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };

        // override osg::Geode
        virtual const char *libraryName() const override;
        virtual const char *className() const override;

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

        Level              &mLevel;
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
    };

    typedef osg::ref_ptr<Layer> LayerPtr;

}


#endif /* LAYER_H_ */

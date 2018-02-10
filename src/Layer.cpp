/*
 * Layer.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "Layer.h"

#include <osgUtil/SmoothingVisitor>
#include <osg/Texture2D>

#include "Level.h"
#include "OdDefines.h"

// yeah, i know these are unintuitive at first. but they are kinda shorter
#define OD_LAYER_FLAG_DIV_BACKSLASH 1

namespace od
{

    Layer::Layer(Level &level)
    : mLevel(level)
    , mId(0)
    , mWidth(0)
    , mHeight(0)
    , mType(TYPE_FLOOR)
    , mOriginX(0)
    , mOriginZ(0)
    , mWorldHeight(0)
    , mLayerName("")
    , mFlags(0)
    , mLightDirection(0)
    , mLightAscension(0)
    , mLightColor(0)
    , mAmbientColor(0)
    , mLightDropoffType(DROPOFF_NONE)
    {
    }

    void Layer::loadDefinition(DataReader &dr)
    {
        dr  >> mId
            >> mWidth
            >> mHeight;

        uint32_t type;
        dr >> type;
        mType = static_cast<LayerType>(type);

        dr  >> mOriginX
            >> mOriginZ
            >> mWorldHeight
            >> mLayerName
            >> mFlags
            >> mLightDirection
            >> mLightAscension
            >> mLightColor
            >> mAmbientColor;

        uint32_t lightDropoffType;
        dr >> lightDropoffType;
        mLightDropoffType = static_cast<LightDropoffType>(lightDropoffType);

        uint32_t visibleLayerCount;
        dr >> visibleLayerCount;
        mVisibleLayers.reserve(visibleLayerCount);
        for(size_t i = 0; i < visibleLayerCount; ++i)
        {
            uint32_t v;
            dr >> v;
            mVisibleLayers.push_back(v);
        }
    }

    void Layer::loadPolyData(DataReader &dr)
    {
        mVertices.reserve((mWidth+1)*(mHeight+1));
        for(size_t i = 0; i < (mWidth+1)*(mHeight+1); ++i)
        {
            uint8_t vertexType;
            uint16_t heightOffsetBiased;

            dr >> vertexType
               >> DataReader::Ignore(1)
               >> heightOffsetBiased;

            Vertex v;
            v.type = vertexType;
            v.heightOffset = (heightOffsetBiased - 0x8000) * 2;
            v.absoluteHeight = OD_WORLD_SCALE*(mWorldHeight + v.heightOffset);

            mVertices.push_back(v);
        }

        mCells.reserve(mWidth*mHeight);
        for(size_t i = 0; i < mWidth*mHeight; ++i)
        {
            Cell c;

            dr >> c.flags
               >> c.leftTextureRef
               >> c.rightTextureRef;

            for(size_t j = 0; j < 8; ++j)
            {
                dr >> c.texCoords[j];
            }

            mCells.push_back(c);
        }

    }

    void Layer::buildTextureAtlas(TextureAtlas &atlas)
    {
        Logger::debug() << "Building texture atlas for layer " << mId;

        for(size_t i = 0; i < mWidth*mHeight; ++i)
        {
            Cell c = mCells[i];

            if(!c.leftTextureRef.isNull())
            {
                TexturePtr tex = mLevel.getAssetAsTexture(c.leftTextureRef);
                atlas.addTexture(c.leftTextureRef, tex);
            }

            if(!c.rightTextureRef.isNull())
            {
                TexturePtr tex = mLevel.getAssetAsTexture(c.rightTextureRef);
                atlas.addTexture(c.rightTextureRef, tex);
            }
        }

        atlas.build();
    }

    void Layer::buildGeometry()
    {
        Logger::debug() << "Building geometry for layer " << mId;

        // for now, build our own texture atlas for every layer here. we can decide to build a level-wide one later
        mTextureAtlas = osg::ref_ptr<TextureAtlas>(new TextureAtlas);
        this->buildTextureAtlas(*mTextureAtlas);

        mGeometry = osg::ref_ptr<osg::Geometry>(new osg::Geometry);

        // iterate over triangles and generate vertices and UVs
        osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
        osg::ref_ptr<osg::Vec2Array> uvCoords(new osg::Vec2Array);
        vertices->reserve(mWidth*mHeight*6); // each cell has 2 triangles, each triangle has 3 vertices
        uvCoords->reserve(mWidth*mHeight*6);
        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            Cell cell = mCells[cellIndex];
            AssetRef textureRef = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

            // skip triangles with no texture on them. they'd be invisible anyway
            if(textureRef.isNull())
            {
                continue;
            }

            int aXRel = cellIndex%mWidth;
            int aZRel = cellIndex/mWidth;// has to be an integer operation to floor it
            float aX = mOriginX + aXRel;
            float aZ = mOriginZ + aZRel;

            // calculate indices of corner vertices
            // z x> --a------b---
            // V      | cell | cell
            //        |  #n  | #n+1
            //      --c------d---
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (mWidth+1); // one row below a, one row contains width+1 vertices
            size_t d = b + (mWidth+1);

            osg::Vec3 aVec = osg::Vec3(  aX, mVertices[a].absoluteHeight,   aZ);
            osg::Vec3 bVec = osg::Vec3(aX+1, mVertices[b].absoluteHeight,   aZ);
            osg::Vec3 cVec = osg::Vec3(  aX, mVertices[c].absoluteHeight, aZ+1);
            osg::Vec3 dVec = osg::Vec3(aX+1, mVertices[d].absoluteHeight, aZ+1);

            osg::Vec2 aUvVec;
            osg::Vec2 bUvVec;
            osg::Vec2 cUvVec;
            osg::Vec2 dUvVec;
            std::tie(aUvVec, bUvVec, cUvVec, dUvVec) = mTextureAtlas->getUvOfTexture(textureRef);

            if(!(mCells[cellIndex].flags & OD_LAYER_FLAG_DIV_BACKSLASH))
            {
                if(isLeft)
                {
                    vertices->push_back(cVec);
                    vertices->push_back(bVec);
                    vertices->push_back(aVec);

                    uvCoords->push_back(cUvVec);
                    uvCoords->push_back(bUvVec);
                    uvCoords->push_back(aUvVec);

                }else
                {
                    vertices->push_back(cVec);
                    vertices->push_back(dVec);
                    vertices->push_back(bVec);

                    uvCoords->push_back(cUvVec);
                    uvCoords->push_back(dUvVec);
                    uvCoords->push_back(bUvVec);
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    vertices->push_back(aVec);
                    vertices->push_back(cVec);
                    vertices->push_back(dVec);

                    uvCoords->push_back(aUvVec);
                    uvCoords->push_back(cUvVec);
                    uvCoords->push_back(dUvVec);

                }else
                {
                    vertices->push_back(aVec);
                    vertices->push_back(dVec);
                    vertices->push_back(bVec);

                    uvCoords->push_back(aUvVec);
                    uvCoords->push_back(dUvVec);
                    uvCoords->push_back(bUvVec);
                }
            }
        }
        mGeometry->setVertexArray(vertices);
        mGeometry->setTexCoordArray(0, uvCoords);
        mGeometry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::TRIANGLES, 0, vertices->size()));

        // add color
        osg::ref_ptr<osg::Vec4Array> colorArray(new osg::Vec4Array());
        colorArray->push_back(osg::Vec4(1,1,1,1));
        mGeometry->setColorArray(colorArray);
        mGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

        // setup texture
        osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
        texture->setImage(mTextureAtlas);
        texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
        texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
        texture->setResizeNonPowerOfTwoHint(false);
        mGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

        if(mGeometry->checkForDeprecatedData())
        {
            Logger::warn() << "Layer " << mId << " geometry contains deprecated data";
        }

        this->addDrawable(mGeometry);

        // generate normals
        osgUtil::SmoothingVisitor sm;
        this->accept(sm);
    }

    const char *Layer::libraryName() const
    {
        return OD_LIB_NAME;
    }

    const char *Layer::className() const
    {
        return "Layer";
    }

}


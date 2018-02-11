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
    , mVisibleTriangles(0)
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

            if(!c.leftTextureRef.isNull())
            {
            	++mVisibleTriangles;
            }

            if(!c.rightTextureRef.isNull())
            {
            	++mVisibleTriangles;
            }
        }
    }

    void Layer::buildGeometry()
    {
        Logger::debug() << "Building geometry for layer " << mId;

        // generate vertices and UVs for SegmentedGeode

        std::vector<osg::Vec3> vertices;
        vertices.reserve(mVertices.size());
        for(size_t i = 0; i < mVertices.size(); ++i)
        {
        	size_t aXRel = i%(mWidth+1);
            size_t aZRel = i/(mWidth+1); // has to be an integer operation to floor it
            float aX = mOriginX + aXRel;
            float aZ = mOriginZ + aZRel;

            vertices.push_back(osg::Vec3(aX, mVertices[i].absoluteHeight, aZ));
        }

        std::vector<Face> faces;
        faces.reserve(mVisibleTriangles);
        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            Cell cell = mCells[cellIndex];
            Face f;
        	f.vertexCount = 3;
        	f.texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

        	if(f.texture.isNull())
        	{
        		continue;
        	}

            int aZRel = cellIndex/mWidth; // has to be an integer operation to floor it

            // calculate indices of corner vertices
            // z x> --a------b---
            // V      | cell | cell
            //        |  #n  | #n+1
            //      --c------d---
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (mWidth+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            // this order seems to work in most cases, but sometimes texture orientations are still messed up
            osg::Vec2 uvA(cell.texCoords[0]/0xffff, cell.texCoords[1]/0xffff);
            osg::Vec2 uvB(cell.texCoords[6]/0xffff, cell.texCoords[7]/0xffff);
            osg::Vec2 uvC(cell.texCoords[4]/0xffff, cell.texCoords[5]/0xffff);
            osg::Vec2 uvD(cell.texCoords[2]/0xffff, cell.texCoords[3]/0xffff);

            if(!(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH))
            {
                if(isLeft)
                {
                    f.vertexIndices[0] = c;
                    f.vertexIndices[1] = b;
                    f.vertexIndices[2] = a;
                    f.vertexUvCoords[0] = uvC;
                    f.vertexUvCoords[1] = uvB;
                    f.vertexUvCoords[2] = uvA;

                }else
                {
                	f.vertexIndices[0] = c;
                    f.vertexIndices[1] = d;
                    f.vertexIndices[2] = b;
                    f.vertexUvCoords[0] = uvC;
                    f.vertexUvCoords[1] = uvD;
                    f.vertexUvCoords[2] = uvB;
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    f.vertexIndices[0] = a;
                    f.vertexIndices[1] = c;
                    f.vertexIndices[2] = d;
                    f.vertexUvCoords[0] = uvA;
                    f.vertexUvCoords[1] = uvC;
                    f.vertexUvCoords[2] = uvD;

                }else
                {
                    f.vertexIndices[0] = a;
                    f.vertexIndices[1] = d;
                    f.vertexIndices[2] = b;
                    f.vertexUvCoords[0] = uvA;
                    f.vertexUvCoords[1] = uvD;
                    f.vertexUvCoords[2] = uvB;
                }
            }

            faces.push_back(f);
        }

        build(mLevel, vertices, faces);

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


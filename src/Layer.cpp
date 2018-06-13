/*
 * Layer.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "Layer.h"

#include <osg/Texture2D>
#include <osg/FrontFace>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include "Level.h"
#include "GeodeBuilder.h"

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

            if(!c.leftTextureRef.isNullLayerTexture())
            {
            	++mVisibleTriangles;
            }

            if(!c.rightTextureRef.isNullLayerTexture())
            {
            	++mVisibleTriangles;
            }
        }
    }

    void Layer::buildGeometry(osg::Group *layerGroup)
    {
        Logger::debug() << "Building geometry for layer " << mId;

        // generate vertices and UVs for SegmentedGeode

        GeodeBuilder gb("layer " + mLayerName, mLevel);
        gb.setClampTextures(true);

        std::vector<osg::Vec3> vertices; // TODO: use internal vectors of GeodeBuilder. here we create two redundant vectors
        vertices.reserve(mVertices.size());
        for(size_t i = 0; i < mVertices.size(); ++i)
        {
        	size_t aXRel = i%(mWidth+1);
            size_t aZRel = i/(mWidth+1); // has to be an integer operation to floor it
            float aX = mOriginX + aXRel;
            float aZ = mOriginZ + aZRel;

            vertices.push_back(osg::Vec3(aX, mVertices[i].absoluteHeight, aZ));
        }
        gb.setVertexVector(vertices.begin(), vertices.end());

        std::vector<Polygon> polygons;
        polygons.reserve(mVisibleTriangles);
        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            Cell cell = mCells[cellIndex];
            Polygon poly;
        	poly.vertexCount = 3;
        	poly.texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;
        	poly.doubleSided = false; // since for "between"-layers, all polys are double sided it is probably
        	                          //  more efficient to just deactivate face culling for them

        	if(poly.texture.isNullLayerTexture())
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
                    poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = b;
                    poly.vertexIndices[2] = a;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvB;
                    poly.uvCoords[2] = uvA;

                }else
                {
                	poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = c;
                    poly.vertexIndices[2] = d;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvC;
                    poly.uvCoords[2] = uvD;

                }else
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }
            }

            polygons.push_back(poly);
        }
        gb.setPolygonVector(polygons.begin(), polygons.end());

        mLayerGeode = new osg::Geode;
        mLayerGeode->setName("layer " + mLayerName);
        gb.build(mLayerGeode);
        layerGroup->addChild(mLayerGeode);

        if(mType == TYPE_FLOOR)
        {
        	mLayerGeode->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::COUNTER_CLOCKWISE), osg::StateAttribute::ON);

        }else if(mType == TYPE_CEILING)
		{
			mLayerGeode->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);

		}else if(mType == TYPE_BETWEEN)
		{
			// so both faces get rendered. that's what this "between" is, right?
			mLayerGeode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
		}
    }

    btCollisionShape *Layer::getCollisionShape()
    {
        if(mCollisionShape != nullptr)
        {
            return mCollisionShape.get();
        }

        if(mVisibleTriangles == 0)
        {
        	return nullptr;
        }

        bool mustUse32BitIndices = (mVertices.size() - 1 > 0xffff); // should save us some memory most of the time
        mBulletMesh.reset(new btTriangleMesh(mustUse32BitIndices, false));
        btTriangleMesh *mesh = mBulletMesh.get(); // because we call members very often and unique_ptr has some overhead

        // first, add all vertices in grid to shape
        mesh->preallocateVertices(mVertices.size() * 3); // bullet seems to be buggy here. it actually needs 3 times the space it reserves
        for(size_t i = 0; i < mVertices.size(); ++i)
        {
            size_t aXRel = i%(mWidth+1);
            size_t aZRel = i/(mWidth+1); // has to be an integer operation to floor it
            float aX = aXRel; // ignore origin so shape is relative to layer origin. we place it in world coords via the collision object
            float aZ = aZRel;

            mesh->findOrAddVertex(btVector3(aX, mVertices[i].heightOffset * OD_WORLD_SCALE, aZ), false);
        }

        // second, push indices for each triangle, ignoring those without texture as these define holes the player can walk/fall through
        mesh->preallocateIndices(mVisibleTriangles * 3);
        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            Cell cell = mCells[cellIndex];
            AssetRef texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

            if(texture.isNullLayerTexture())
            {
                continue;
            }

            int aZRel = cellIndex/mWidth; // has to be an integer operation to floor it

            // calculate indices of corner vertices. same as in buildGeometry()
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (mWidth+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            if(!(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH))
            {
                if(isLeft)
                {
                    mesh->addTriangleIndices(c, b, a);

                }else
                {
                    mesh->addTriangleIndices(c, d, b);
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    mesh->addTriangleIndices(a, c, d);

                }else
                {
                    mesh->addTriangleIndices(a, d, b);
                }
            }
        }

        mCollisionShape.reset(new btBvhTriangleMeshShape(mesh, true, true));

        return mCollisionShape.get();
    }
}


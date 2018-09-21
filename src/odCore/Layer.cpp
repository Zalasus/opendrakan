/*
 * Layer.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include <odCore/Layer.h>

#include <limits>
#include <osg/Texture2D>
#include <osg/FrontFace>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <odCore/Level.h>
#include <odCore/GeodeBuilder.h>
#include <odCore/NodeMasks.h>
#include <odCore/Engine.h>

// yeah, i know these are unintuitive at first. but they are kinda shorter
#define OD_LAYER_FLAG_DIV_BACKSLASH 1

namespace od
{

    const AssetRef Layer::HoleTextureRef(0xffff, 0xffff);
    const AssetRef Layer::InvisibleTextureRef(0xfffe, 0xffff);


    Layer::Layer(Level &level)
    : mLevel(level)
    , mId(0)
    , mWidth(0)
    , mHeight(0)
    , mType(TYPE_FLOOR)
    , mOriginX(0)
    , mOriginZ(0)
    , mWorldHeightWu(0)
    , mLayerName("")
    , mFlags(0)
    , mLightDirection(0)
    , mLightAscension(0)
    , mLightDropoffType(DROPOFF_NONE)
    , mVisibleTriangles(0)
    {
        this->setNodeMask(NodeMasks::Layer);
    }

    Layer::~Layer()
    {
        if(mLightCallback != nullptr)
        {
            this->removeCullCallback(mLightCallback);
        }
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
            >> mWorldHeightWu
            >> mLayerName
            >> mFlags
            >> mLightDirection
            >> mLightAscension;

        uint32_t lightColor;
        uint32_t ambientColor;
        dr  >> lightColor
            >> ambientColor;

        mLightColor.set(((lightColor >> 16) & 0xff)/255.0, ((lightColor >> 8) & 0xff)/255.0, (lightColor & 0xff)/255.0);
        mAmbientColor.set(((ambientColor >> 16) & 0xff)/255.0, ((ambientColor >> 8) & 0xff)/255.0, (ambientColor & 0xff)/255.0);
        mLightDirectionVector.set(
                std::cos(mLightDirection)*std::cos(mLightAscension),
                std::sin(mLightAscension),
               -std::sin(mLightDirection)*std::cos(mLightAscension));

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
            v.heightOffsetLu = OD_WORLD_SCALE*(heightOffsetBiased - 0x8000)*2;

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

            if(c.leftTextureRef != HoleTextureRef && c.leftTextureRef != InvisibleTextureRef)
            {
            	++mVisibleTriangles;
            }

            if(c.rightTextureRef != HoleTextureRef && c.rightTextureRef != InvisibleTextureRef)
            {
            	++mVisibleTriangles;
            }
        }
    }

    void Layer::buildGeometry()
    {
        Logger::debug() << "Building geometry for layer " << mId;

        // generate vertices and UVs for SegmentedGeode

        GeodeBuilder gb("layer " + mLayerName, mLevel);
        gb.setClampTextures(true);
        gb.setNormalsFromCcw(true);

        std::vector<osg::Vec3> vertices; // TODO: use internal vectors of GeodeBuilder. here we create two redundant vectors
        vertices.reserve(mVertices.size());
        for(size_t i = 0; i < mVertices.size(); ++i)
        {
        	size_t aXRel = i%(mWidth+1);
            size_t aZRel = i/(mWidth+1); // has to be an integer operation to floor it

            vertices.push_back(osg::Vec3(aXRel, mVertices[i].heightOffsetLu, aZRel));
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
        	poly.doubleSided = (mType == TYPE_BETWEEN);

        	if(poly.texture == HoleTextureRef || poly.texture == InvisibleTextureRef)
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

            osg::Vec2 uvA(cell.texCoords[3]/0xffff, cell.texCoords[7]/0xffff);
            osg::Vec2 uvB(cell.texCoords[2]/0xffff, cell.texCoords[6]/0xffff);
            osg::Vec2 uvC(cell.texCoords[0]/0xffff, cell.texCoords[4]/0xffff);
            osg::Vec2 uvD(cell.texCoords[1]/0xffff, cell.texCoords[5]/0xffff);

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

            if(mType == TYPE_CEILING)
            {
                // swap two vertices, thus reversing the winding order
                std::swap(poly.vertexIndices[0], poly.vertexIndices[1]);
                std::swap(poly.uvCoords[0], poly.uvCoords[1]);
            }

            polygons.push_back(poly);
        }
        gb.setPolygonVector(polygons.begin(), polygons.end());

        this->setPosition(osg::Vec3(mOriginX, getWorldHeightLu(), mOriginZ));
        this->setName("layer " + mLayerName);

        mLayerGeode = new osg::Geode;
        gb.build(mLayerGeode);
        this->addChild(mLayerGeode);

        mLightCallback = new LightStateCallback(mLevel.getEngine().getLightManager(), mLayerGeode, true);
        mLightCallback->setLayerLight(mLightColor, mAmbientColor, mLightDirectionVector);
        mLightCallback->lightingDirty();
        this->addCullCallback(mLightCallback);
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

            mesh->findOrAddVertex(btVector3(aX, mVertices[i].heightOffsetLu, aZ), false);
        }

        // second, push indices for each triangle, ignoring those without texture as these define holes the player can walk/fall through
        mesh->preallocateIndices(mVisibleTriangles * 3);
        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            Cell cell = mCells[cellIndex];
            AssetRef texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

            if(texture == HoleTextureRef) // unlike when building geometry, we want to include invisible triangles here!
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

    bool Layer::hasHoleAt(const osg::Vec2 &absolutePos)
    {
        if(!contains(absolutePos))
        {
            return false;
        }

        osg::Vec2 relativePos = absolutePos - osg::Vec2(mOriginX, mOriginZ);

        // for points directly on the layer's border, allow a certain epsilon so we don't run into undefined cells
        float epsilon = 0.00001;
        if(relativePos.x() == mWidth)
        {
            relativePos.x() -= epsilon;

        }else if(relativePos.x() <= 0)
        {
            relativePos.x() += epsilon;
        }

        if(relativePos.y() == mHeight)
        {
            relativePos.y() -= epsilon;

        }else if(relativePos.y() <= 0)
        {
            relativePos.y() += epsilon;
        }

        float cellX = std::floor(relativePos.x());
        float cellZ = std::floor(relativePos.y());
        float fractX = relativePos.x() - cellX;
        float fractZ = relativePos.y() - cellZ;

        size_t cellIndex = cellX + cellZ*mWidth;
        if(cellIndex >= mCells.size())
        {
            // should we just return false here?
            throw Exception("Calculated cell index lies outside of cell array. Seems like the layer bounds check is incorrect");
        }

        Cell &cell = mCells[cellIndex];

        // we have the cell. now find the triangle to which the fractional part belongs
        bool isLeftTriangle;
        if(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH)
        {
            isLeftTriangle = (fractX < fractZ);

        }else
        {
            isLeftTriangle = (1 - fractX > fractZ);
        }

        AssetRef texture = isLeftTriangle ? cell.leftTextureRef : cell.rightTextureRef;

        return texture == HoleTextureRef;
    }

    bool Layer::contains(const osg::Vec2 &xzCoord)
    {
        return xzCoord.x() >= mOriginX && xzCoord.x() <= (mOriginX+mWidth)
                && xzCoord.y() >= mOriginZ && xzCoord.y() <= (mOriginZ+mHeight);
    }

    bool Layer::contains(const osg::Vec2 &xzCoord, float epsilon)
    {
        return xzCoord.x() >= (mOriginX-epsilon) && xzCoord.x() <= (mOriginX+mWidth+epsilon)
                && xzCoord.y() >= (mOriginZ-epsilon) && xzCoord.y() <= (mOriginZ+mHeight+epsilon);
    }

    float Layer::getAbsoluteHeightAt(const osg::Vec2 &xzCoord)
    {
        if(!contains(xzCoord))
        {
            return NAN;
        }

        osg::Vec2 relativePos = xzCoord - osg::Vec2(mOriginX, mOriginZ);

        // for points directly on the layer's border, allow a certain epsilon so we don't run into undefined cells
        float epsilon = 0.00001;
        if(relativePos.x() == mWidth)
        {
            relativePos.x() -= epsilon;

        }else if(relativePos.x() <= 0)
        {
            relativePos.x() += epsilon;
        }

        if(relativePos.y() == mHeight)
        {
            relativePos.y() -= epsilon;

        }else if(relativePos.y() <= 0)
        {
            relativePos.y() += epsilon;
        }

        size_t cellX = relativePos.x();
        size_t cellZ = relativePos.y();
        float fractX = relativePos.x() - cellX;
        float fractZ = relativePos.y() - cellZ;

        size_t cellIndex = cellX + cellZ*mWidth;
        if(cellIndex >= mCells.size())
        {
            throw Exception("Calculated cell index lies outside of cell array. Seems like the layer bounds check is incorrect");
        }

        Cell &cell = mCells[cellIndex];

        // calculate indices of corner vertices. same as in buildGeometry()
        size_t a = cellIndex + cellZ; // add row index since we want to skip top right vertex in every row passed so far
        size_t b = a + 1;
        size_t c = a + (mWidth+1); // one row below a, one row contains width+1 vertices
        size_t d = c + 1;
        float ya = mVertices[a].heightOffsetLu;
        float yb = mVertices[b].heightOffsetLu;
        float yc = mVertices[c].heightOffsetLu;
        float yd = mVertices[d].heightOffsetLu;

        // calculate height using generic formula for a 3D plane for which we just need to determine the
        //  coefficients depending on where in the cell we are
        float heightAnchor;
        float dx;
        float dz;
        float heightDeltaX;
        float heightDeltaZ;
        if(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH)
        {
            bool isLeftTriangle = (fractX < fractZ);
            if(isLeftTriangle)
            {
                dx =    fractX;
                dz = (1-fractZ);

                heightDeltaX = (yd-yc);
                heightDeltaZ = (ya-yc);

                heightAnchor = yc;

            }else
            {
                dx = (1-fractX);
                dz = fractZ;

                heightDeltaX = (ya-yb);
                heightDeltaZ = (yd-yb);

                heightAnchor = yb;
            }

        }else
        {
            bool isLeftTriangle = (1 - fractX > fractZ);
            if(isLeftTriangle)
            {
                dx = fractX;
                dz = fractZ;

                heightDeltaX = (yb-ya);
                heightDeltaZ = (yc-ya);

                heightAnchor = ya;

            }else
            {
                dx = (1-fractX);
                dz = (1-fractZ);

                heightDeltaX = (yc-yd);
                heightDeltaZ = (yb-yd);

                heightAnchor = yd;
            }
        }

        return getWorldHeightLu() + heightAnchor + dx*heightDeltaX + dz*heightDeltaZ;
    }

}


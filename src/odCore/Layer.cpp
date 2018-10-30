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
#include <odCore/render/RenderManager.h>
#include <odCore/render/ShaderFactory.h>

// yeah, i know these are unintuitive at first. but they are kinda shorter
#define OD_LAYER_FLAG_DIV_BACKSLASH 1

namespace od
{

    const odDb::AssetRef Layer::HoleTextureRef(0xffff, 0xffff);
    const odDb::AssetRef Layer::InvisibleTextureRef(0xfffe, 0xffff);


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
        float lowestHeightOffset = std::numeric_limits<float>::max();
        float maxHeightOffset = std::numeric_limits<float>::lowest();
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

            if(v.heightOffsetLu < lowestHeightOffset)
            {
                lowestHeightOffset = v.heightOffsetLu;
            }

            if(v.heightOffsetLu > maxHeightOffset)
            {
                maxHeightOffset = v.heightOffsetLu;
            }

            mVertices.push_back(v);
        }

        osg::Vec3 min(mOriginX, getWorldHeightLu()+lowestHeightOffset, mOriginZ);
        osg::Vec3 max(mOriginX+mWidth, getWorldHeightLu()+maxHeightOffset, mOriginZ+mHeight);
        mBoundingBox = osg::BoundingBox(min, max);

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

        mLayerGeode = new osg::Geode;
        gb.build(mLayerGeode);

        // store references to the shared arrays of the generated geometries. we need them to bake the layer lighting
        mGeometryVertexArray = gb.getBuiltVertexArray();
        mGeometryNormalArray = gb.getBuiltNormalArray();
        mGeometryColorArray  = gb.getBuiltColorArray();
        _bakeLocalLayerLight();

        this->setPosition(osg::Vec3(mOriginX, getWorldHeightLu(), mOriginZ));
        this->setName("layer " + mLayerName);
        this->addChild(mLayerGeode);

        osg::ref_ptr<osg::Program> layerProg = mLevel.getEngine().getRenderManager().getShaderFactory().getProgram("layer");
        mLayerGeode->getOrCreateStateSet()->setAttribute(layerProg);

        mLightCallback = new odRender::LightStateCallback(mLevel.getEngine().getRenderManager(), mLayerGeode, true);
        mLightCallback->setLayerLight(osg::Vec3(), osg::Vec3(), osg::Vec3()); // disable layer light. we bake it into the color array
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
            odDb::AssetRef texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;

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

        odDb::AssetRef texture = isLeftTriangle ? cell.leftTextureRef : cell.rightTextureRef;

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

    static int32_t clamp(int32_t val, int32_t min, int32_t max)
    {
        return val < min ? min : (val > max ? max : val);
    }

    static float clamp(float val, float min, float max)
    {
        return val < min ? min : (val > max ? max : val);
    }

    static osg::Vec4 clamp(osg::Vec4 val, osg::Vec4::value_type min, osg::Vec4::value_type max)
    {
        for(size_t i = 0; i < 4; ++i)
        {
            val[i] = clamp(val[i], min, max);
        }

        return val;
    }

    void Layer::_bakeLocalLayerLight()
    {
        if(mGeometryNormalArray->size() != mGeometryVertexArray->size())
        {
            throw Exception("Bad generated geometry arrays. Normal and vertex array sizes must match for baking lighting");
        }

        mGeometryColorArray->resize(mGeometryVertexArray->size(), osg::Vec4());
        mGeometryColorArray->setBinding(osg::Array::BIND_PER_VERTEX);

        for(size_t i = 0; i < mGeometryVertexArray->size(); ++i)
        {
            // for some reason, the Riot Engine seems to add the ambient component twice in layer lighting
            osg::Vec3::value_type cosTheta = std::max((*mGeometryNormalArray)[i]*mLightDirectionVector, (osg::Vec3::value_type)0.0);
            osg::Vec3 lightColor = mLightColor*cosTheta + mAmbientColor*2;

            (*mGeometryColorArray)[i] = osg::Vec4(lightColor, 1.0);

            // also store the color in the vertex array. blending lighting of adjacent layers can't operate in-place on color array
            uint32_t x = std::round((*mGeometryVertexArray)[i].x());
            uint32_t z = std::round((*mGeometryVertexArray)[i].z());
            size_t vertIndex = x + z*(mWidth+1);
            mVertices.at(vertIndex).calculatedLightColor = lightColor;
        }
    }

    void Layer::bakeOverlappingLayerLighting()
    {
        // FIXME: this function is a mess. clean it up!

        if(mGeometryColorArray == nullptr)
        {
            throw Exception("Need to build geometry before calling bakeOverlappingLayerLighting()");
        }

        // build overlap map
        std::vector<Layer*> overlappingLayers;
        overlappingLayers.reserve(10);
        mLevel.findAdjacentAndOverlappingLayers(this, overlappingLayers);

        struct VertexOverlap
        {
            VertexOverlap() : layerCount(0) {}
            void addLayer(Layer *layer)
            {
                if(layerCount < (sizeof(layers)/sizeof(layers[0])))
                {
                    layers[layerCount] = layer;
                    ++layerCount;
                }
            }
            size_t layerCount;
            Layer* layers[4];
        };
        std::vector<VertexOverlap> overlapMap(mVertices.size());

        for(auto layerIt = overlappingLayers.begin(); layerIt != overlappingLayers.end(); ++layerIt)
        {
            Layer *layer = *layerIt;

            // for every overlapping layer, find xz range of overlapping vertices
            int32_t relOriginX = layer->getOriginX() - mOriginX;
            int32_t relOriginZ = layer->getOriginZ() - mOriginZ;
            uint32_t xStart = clamp(relOriginX, 0, mWidth);
            uint32_t zStart = clamp(relOriginZ, 0, mHeight);
            uint32_t xEnd =   clamp(relOriginX + layer->getWidth(), 0, mWidth);
            uint32_t zEnd =   clamp(relOriginZ + layer->getHeight(), 0, mHeight);

            // for each vertex in that range, check for vertices within height threshold
            for(uint32_t z = zStart; z <= zEnd; ++z)
            {
                for(uint32_t x = xStart; x <= xEnd; ++x)
                {
                    size_t ourVertIndex = x + z*(mWidth+1);
                    size_t theirVertIndex = (x - relOriginX) + (z - relOriginZ)*(layer->getWidth()+1);

                    float ourVertexHeight = mVertices.at(ourVertIndex).heightOffsetLu + getWorldHeightLu();
                    float theirVertexHeight = layer->mVertices.at(theirVertIndex).heightOffsetLu + layer->getWorldHeightLu();

                    if(std::abs(ourVertexHeight - theirVertexHeight) <= 2*OD_WORLD_SCALE) // threshold is +/- 2wu
                    {
                        // vertices are within range! add to list
                        overlapMap.at(ourVertIndex).addLayer(layer);
                    }
                }
            }
        }

        for(size_t i = 0; i < mGeometryVertexArray->size(); ++i)
        {
            // find this vertex in overlap map. note that we can't use the index here, as the geometry's vertex array potentially
            //  contains more vertices than the layer itself. we need to derive x and z from the vertex coordinates
            uint32_t x = std::round((*mGeometryVertexArray)[i].x());
            uint32_t z = std::round((*mGeometryVertexArray)[i].z());
            size_t vertIndex = x + z*(mWidth+1);

            VertexOverlap &overlap = overlapMap.at(vertIndex);
            for(size_t layerIndex = 0; layerIndex < overlap.layerCount; ++layerIndex)
            {
                Layer *layer = overlap.layers[layerIndex];
                if(layer == nullptr)
                {
                    continue;
                }

                int32_t relOriginX = layer->getOriginX() - mOriginX;
                int32_t relOriginZ = layer->getOriginZ() - mOriginZ;
                size_t theirVertIndex = (x - relOriginX) + (z - relOriginZ)*(layer->getWidth()+1);

                (*mGeometryColorArray)[i] += osg::Vec4(layer->mVertices.at(theirVertIndex).calculatedLightColor, 0.0);
            }

            (*mGeometryColorArray)[i] = clamp((*mGeometryColorArray)[i], 0.0, 1.0) / (overlap.layerCount + 1);
        }
    }

}


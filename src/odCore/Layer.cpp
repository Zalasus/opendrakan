/*
 * Layer.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include <odCore/Layer.h>

#include <limits>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <glm/common.hpp>

#include <odCore/Level.h>
#include <odCore/Engine.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Geometry.h>

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
    }

    Layer::~Layer()
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
            >> mWorldHeightWu
            >> mLayerName
            >> mFlags
            >> mLightDirection
            >> mLightAscension;

        uint32_t lightColor;
        uint32_t ambientColor;
        dr  >> lightColor
            >> ambientColor;

        mLightColor.r = ((lightColor >> 16) & 0xff)/255.0;
        mLightColor.g = ((lightColor >> 8) & 0xff)/255.0;
        mLightColor.b = (lightColor & 0xff)/255.0;

        mAmbientColor.r = ((ambientColor >> 16) & 0xff)/255.0;
        mAmbientColor.g = ((ambientColor >> 8) & 0xff)/255.0;
        mAmbientColor.b = ((ambientColor & 0xff)/255.0);

        mLightDirectionVector.x = std::cos(mLightDirection)*std::cos(mLightAscension);
        mLightDirectionVector.y = std::sin(mLightAscension);
        mLightDirectionVector.z = -std::sin(mLightDirection)*std::cos(mLightAscension);

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

        glm::vec3 min(mOriginX, getWorldHeightLu()+lowestHeightOffset, mOriginZ);
        glm::vec3 max(mOriginX+mWidth, getWorldHeightLu()+maxHeightOffset, mOriginZ+mHeight);
        mBoundingBox = AxisAlignedBoundingBox(min, max);

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

    void Layer::spawn()
    {
        odRender::Renderer *renderer = mLevel.getEngine().getRenderer();
        if(renderer != nullptr)
        {
            mLayerNode = renderer->createLayerNode(this);
        }

        _bakeLocalLayerLight();
    }

    void Layer::despawn()
    {
        mLayerNode = nullptr;
    }

    bool Layer::hasHoleAt(const glm::vec2 &absolutePos)
    {
        if(!contains(absolutePos))
        {
            return false;
        }

        glm::vec2 relativePos = absolutePos - glm::vec2(mOriginX, mOriginZ);

        // for points directly on the layer's border, allow a certain epsilon so we don't run into undefined cells
        float epsilon = 0.00001;
        if(relativePos.x == mWidth)
        {
            relativePos.x -= epsilon;

        }else if(relativePos.x <= 0)
        {
            relativePos.x += epsilon;
        }

        if(relativePos.y == mHeight)
        {
            relativePos.y -= epsilon;

        }else if(relativePos.y <= 0)
        {
            relativePos.y += epsilon;
        }

        float cellX = std::floor(relativePos.x);
        float cellZ = std::floor(relativePos.y);
        float fractX = relativePos.x - cellX;
        float fractZ = relativePos.y - cellZ;

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

    bool Layer::contains(const glm::vec2 &xzCoord)
    {
        return xzCoord.x >= mOriginX && xzCoord.x <= (mOriginX+mWidth)
                && xzCoord.y >= mOriginZ && xzCoord.y <= (mOriginZ+mHeight);
    }

    bool Layer::contains(const glm::vec2 &xzCoord, float epsilon)
    {
        return xzCoord.x >= (mOriginX-epsilon) && xzCoord.x <= (mOriginX+mWidth+epsilon)
                && xzCoord.y >= (mOriginZ-epsilon) && xzCoord.y <= (mOriginZ+mHeight+epsilon);
    }

    float Layer::getAbsoluteHeightAt(const glm::vec2 &xzCoord)
    {
        if(!contains(xzCoord))
        {
            return NAN;
        }

        glm::vec2 relativePos = xzCoord - glm::vec2(mOriginX, mOriginZ);

        // for points directly on the layer's border, allow a certain epsilon so we don't run into undefined cells
        float epsilon = 0.00001;
        if(relativePos.x == mWidth)
        {
            relativePos.x -= epsilon;

        }else if(relativePos.x <= 0)
        {
            relativePos.x += epsilon;
        }

        if(relativePos.y == mHeight)
        {
            relativePos.y -= epsilon;

        }else if(relativePos.y <= 0)
        {
            relativePos.y += epsilon;
        }

        size_t cellX = relativePos.x;
        size_t cellZ = relativePos.y;
        float fractX = relativePos.x - cellX;
        float fractZ = relativePos.y - cellZ;

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

    void Layer::_bakeLocalLayerLight()
    {
        if(mLayerNode == nullptr || mLayerNode->getGeometry() == nullptr)
        {
            return;
        }

        odRender::Geometry *geometry = mLayerNode->getGeometry();
        std::vector<glm::vec3> &vertexArray = geometry->getVertexArray();
        std::vector<glm::vec3> &normalArray = geometry->getNormalArray();
        std::vector<glm::vec4> &colorArray = geometry->getColorArray();

        if(normalArray.size() != vertexArray.size())
        {
            throw Exception("Bad generated geometry arrays. Normal and vertex array sizes must match for baking lighting");
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
            uint32_t xStart = glm::clamp(relOriginX, 0, (int32_t)mWidth);
            uint32_t zStart = glm::clamp(relOriginZ, 0, (int32_t)mHeight);
            uint32_t xEnd =   glm::clamp((int32_t)(relOriginX + layer->getWidth()), 0, (int32_t)mWidth);
            uint32_t zEnd =   glm::clamp((int32_t)(relOriginZ + layer->getHeight()), 0, (int32_t)mHeight);

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

        colorArray.resize(vertexArray.size());

        for(size_t i = 0; i < vertexArray.size(); ++i)
        {
            uint32_t x = std::round(vertexArray[i].x);
            uint32_t z = std::round(vertexArray[i].z);
            size_t vertIndex = x + z*(mWidth+1);

            // calculate average color of this and all overlapping layers at this vertex
            glm::vec3 color = mLightColor;
            glm::vec3 ambient = mAmbientColor;
            VertexOverlap &overlap = overlapMap[vertIndex];
            for(size_t layerIndex = 0; layerIndex < overlap.layerCount; ++layerIndex)
            {
                if(overlap.layers[layerIndex] == nullptr)
                {
                    continue;
                }

                color += overlap.layers[layerIndex]->mLightColor;
                ambient += overlap.layers[layerIndex]->mAmbientColor;
            }
            color /= glm::vec3::value_type(overlap.layerCount + 1);
            ambient /= glm::vec3::value_type(overlap.layerCount + 1);

            // for some reason, the Riot Engine seems to add the ambient component twice in layer lighting
            glm::vec3::value_type cosTheta = std::max(glm::dot(normalArray[i], mLightDirectionVector), (glm::vec3::value_type)0.0);
            glm::vec3 lightColor = color*cosTheta + ambient*glm::vec3::value_type(2);

            colorArray[i] = glm::vec4(lightColor, 1.0);
        }

        geometry->notifyColorDirty();
    }
}


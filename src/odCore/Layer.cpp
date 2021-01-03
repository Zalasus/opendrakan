/*
 * Layer.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include <odCore/Layer.h>

#include <limits>

#include <glm/common.hpp>

#include <odCore/Level.h>
#include <odCore/Light.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Geometry.h>
#include <odCore/render/Array.h>
#include <odCore/render/Model.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

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
    , mCollidingTriangles(0)
    , mMinHeight(0)
    , mMaxHeight(0)
    , mIsSpawned(false)
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
            v.heightOffsetLu = Units::worldUnitsToLengthUnits<float>((heightOffsetBiased - 0x8000)*2);

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

        mMinHeight = lowestHeightOffset + getWorldHeightLu();
        mMaxHeight = maxHeightOffset + getWorldHeightLu();
        glm::vec3 min(mOriginX, mMinHeight, mOriginZ);
        glm::vec3 max(mOriginX+mWidth, mMaxHeight, mOriginZ+mHeight);
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

            if(c.leftTextureRef != HoleTextureRef)
            {
                if(c.leftTextureRef != InvisibleTextureRef)
                {
                    ++mCollidingTriangles;

                }else
                {
                    ++mVisibleTriangles;
                }
            }

            if(c.rightTextureRef != HoleTextureRef)
            {
            	if(c.rightTextureRef != InvisibleTextureRef)
                {
                    ++mCollidingTriangles;

                }else
                {
                    ++mVisibleTriangles;
                }
            }
        }
    }

    void Layer::spawn(odPhysics::PhysicsSystem &physicsSystem, odRender::Renderer *renderer)
    {
        if(renderer != nullptr)
        {
            mRenderHandle = renderer->createHandle(odRender::RenderSpace::LEVEL);
            mRenderModel = renderer->createModelFromLayer(this);

            mRenderHandle->setModel(mRenderModel);
            mRenderHandle->setPosition(getOrigin());

            // global light is baked into vertices, so disable it here. shader should ignore it anyway
            mRenderHandle->setGlobalLight(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,0,0));
        }

        mPhysicsHandle = physicsSystem.createLayerHandle(*this);
        mPhysicsHandle->setLightCallback(this);

        _bakeLocalLayerLight();

        mIsSpawned = true;
    }

    void Layer::despawn()
    {
        mRenderHandle = nullptr;
        mRenderModel = nullptr;
        mPhysicsHandle = nullptr;

        mIsSpawned = false;
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

    void Layer::removeAffectingLight(std::shared_ptr<od::Light> light)
    {
        if(light == nullptr)
        {
            return;
        }

        // let's hope the light is dynamic, cause otherwise we have no way of easily removing the baked light from the vertices

        if(light->isDynamic() && mRenderHandle != nullptr)
        {
            mRenderHandle->removeLight(light);
        }
    }

    void Layer::addAffectingLight(std::shared_ptr<od::Light> light)
    {
        if(light == nullptr)
        {
            return;
        }

        // static lights can be baked into the vertex colors. all others need to be passed to the renderer
        if(!light->isDynamic())
        {
            _bakeStaticLight(*light);

        }else
        {
            if(mRenderHandle != nullptr)
            {
                mRenderHandle->addLight(light);
            }
        }
    }

    void Layer::clearLightList()
    {
        mRenderHandle->clearLightList();
    }

    void Layer::_bakeStaticLight(od::Light &light)
    {
        if(mRenderModel == nullptr || mRenderModel->getGeometryCount() == 0)
        {
            return;
        }

        if(mRenderModel->getGeometryCount() > 1 && !mRenderModel->hasSharedVertexArrays())
        {
            throw od::Exception("Baking layer lighting on models with multiple geometries only works if those share vertex arrays");
        }

        std::shared_ptr<odRender::Geometry> geometry = mRenderModel->getGeometry(0);

        glm::vec3 relLightPosition = light.getPosition() - getOrigin(); // relative to layer origin
        float lightRadius = light.getRadius();
        float lightIntensity = light.getIntensityScaling();
        glm::vec3 lightColor = light.getColor();

        // find maximum rectangular area that can possibly be intersected by the light so we can early-reject vertices
        //  without having to calculate the distance to the light everytime
        int32_t xMin = std::max(relLightPosition.x-lightRadius, 0.0f);
        int32_t xMax = std::min(relLightPosition.x+lightRadius, (float)mWidth);
        int32_t zMin = std::max(relLightPosition.z-lightRadius, 0.0f);
        int32_t zMax = std::max(relLightPosition.z+lightRadius, (float)mHeight);

        odRender::ArrayAccessor<glm::vec3> vertexArray(geometry->getVertexArrayAccessHandler());
        odRender::ArrayAccessor<glm::vec3> normalArray(geometry->getNormalArrayAccessHandler());
        odRender::ArrayAccessor<glm::vec4> colorArray(geometry->getColorArrayAccessHandler());

        for(size_t i = 0; i < vertexArray.size(); ++i)
        {
            glm::vec3 vertexPosition = vertexArray[i];

            if(vertexPosition.x < xMin || vertexPosition.x > xMax ||
               vertexPosition.z < zMin || vertexPosition.z > zMax)
            {
                continue;
            }

            if(glm::length(vertexPosition - relLightPosition) > lightRadius)
            {
                continue;
            }

            glm::vec3 lightDir = relLightPosition - vertexPosition;
            float distance = glm::length(lightDir);
            lightDir /= distance;

            float normDistance = distance/lightRadius;
            float attenuation = -0.82824*normDistance*normDistance - 0.13095*normDistance + 1.01358;
            attenuation = glm::clamp(attenuation, 0.0f, 1.0f);

            float cosTheta = glm::max(glm::dot(normalArray[i], lightDir), 0.0f);

            glm::vec3 newVertexColor = lightIntensity * lightColor * cosTheta * attenuation;
            colorArray[i] += glm::vec4(newVertexColor, 0.0f);
        }
    }

    void Layer::_bakeLocalLayerLight()
    {
        if(mRenderModel == nullptr || mRenderModel->getGeometryCount() == 0)
        {
            return;
        }

        if(mRenderModel->getGeometryCount() > 1 && !mRenderModel->hasSharedVertexArrays())
        {
            throw od::Exception("Baking layer lighting on models with multiple geometries only works if those share vertex arrays");
        }

        std::shared_ptr<odRender::Geometry> geometry = mRenderModel->getGeometry(0);
        odRender::ArrayAccessor<glm::vec3> vertexArray(geometry->getVertexArrayAccessHandler());
        odRender::ArrayAccessor<glm::vec3> normalArray(geometry->getNormalArrayAccessHandler());
        odRender::ArrayAccessor<glm::vec4> colorArray(geometry->getColorArrayAccessHandler());

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

                    if(std::abs(ourVertexHeight - theirVertexHeight) <= Units::worldUnitsToLengthUnits(2)) // threshold is +/- 2wu
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
    }

    void Layer::_calculateNormalsInternal()
    {
        mLocalNormals.resize(mVertices.size(), glm::vec3(0.0));

        for(size_t triIndex = 0; triIndex < mWidth*mHeight*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            od::Layer::Cell cell = mCells[cellIndex];

            int32_t aZRel = cellIndex/mWidth; // has to be an integer operation to floor it

            // calculate indices of corner vertices
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (mWidth+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            size_t centerIndex;
            size_t leftIndex; // as in "used left of cross product"
            size_t rightIndex;

            if(!(cell.flags & OD_LAYER_FLAG_DIV_BACKSLASH))
            {
                if(isLeft)
                {
                    centerIndex = a;
                    leftIndex = c;
                    rightIndex = b;

                }else
                {
                    centerIndex = d;
                    leftIndex = b;
                    rightIndex = c;
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    centerIndex = c;
                    leftIndex = d;
                    rightIndex = a;

                }else
                {
                    centerIndex = b;
                    leftIndex = a;
                    rightIndex = d;
                }
            }

            glm::vec3 center = getVertexAt(centerIndex);
            glm::vec3 left = getVertexAt(leftIndex) - center;
            glm::vec3 right = getVertexAt(rightIndex) - center;
            glm::vec3 normal = glm::cross(left, right);

            mLocalNormals[centerIndex] += normal;
            mLocalNormals[leftIndex] += normal;
            mLocalNormals[rightIndex] += normal;
        }

        for(size_t i = 0; i < mLocalNormals.size(); ++i)
        {
            mLocalNormals[i] = glm::normalize(mLocalNormals[i]);
        }
    }
}

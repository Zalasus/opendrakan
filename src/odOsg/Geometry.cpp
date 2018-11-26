/*
 * Geometry.cpp
 *
 *  Created on: 14 Nov 2018
 *      Author: zal
 */

#include <odOsg/Geometry.h>

#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Texture.h>

namespace odOsg
{

    Geometry::Geometry()
    : mGeode(new osg::Geode)
    , mArrayDataValid(true)
    , mHasBoneInfo(false)
    {
    }

    std::vector<glm::vec3> &Geometry::getVertexArray()
    {
        return mVertexArray;
    }

    std::vector<glm::vec4> &Geometry::getColorArray()
    {
        return mColorArray;
    }

    std::vector<glm::vec3> &Geometry::getNormalArray()
    {
        return mNormalArray;
    }

    std::vector<glm::vec2> &Geometry::getTextureCoordArray()
    {
        return mTextureCoordArray;
    }

    void Geometry::setHasBoneInfo(bool b)
    {
        mHasBoneInfo = b;
    }

    bool Geometry::hasBoneInfo() const
    {
        return mHasBoneInfo;
    }

    std::vector<glm::vec4> &Geometry::getBoneIndexArray()
    {
        return mBoneIndexArray;
    }

    std::vector<glm::vec4> &Geometry::getBoneWeightArray()
    {
        return mBoneWeightArray;
    }

    void Geometry::notifyVertexDataChanged(bool lastUpdate)
    {
        if(lastUpdate)
        {
            mVertexArray.clear();
            mVertexArray.shrink_to_fit();

            mColorArray.clear();
            mColorArray.shrink_to_fit();

            mNormalArray.clear();
            mNormalArray.shrink_to_fit();

            mTextureCoordArray.clear();
            mTextureCoordArray.shrink_to_fit();
        }
    }

    void Geometry::addTexture(Texture *texture)
    {
        mTextures.push_back(texture);
    }

}

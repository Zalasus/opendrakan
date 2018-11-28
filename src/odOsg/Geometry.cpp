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

    void Geometry::notifyColorDirty()
    {
        if(mOsgColorArray == nullptr)
        {
            mOsgColorArray = GlmAdapter::convertToOsgArray<osg::Vec4Array>(mColorArray);

        }else
        {
            mOsgColorArray->resize(mColorArray.size(), osg::Vec4());
            for(size_t i = 0; i < mColorArray.size(); ++i)
            {
                (*mOsgColorArray)[i] = GlmAdapter::toOsg(mColorArray[i]);
            }

            if(mOsgColorArray->size() != mOsgVertexArray->size())
            {
                mOsgColorArray->setBinding(osg::Array::BIND_OVERALL);

            }else
            {
                mOsgColorArray->setBinding(osg::Array::BIND_PER_VERTEX);
            }
        }
    }

    void Geometry::addTexture(Texture *texture)
    {
        mTextures.push_back(texture);
    }

}

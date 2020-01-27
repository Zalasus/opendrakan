/**
 * @file GeometryImpl.cpp
 * @date Jan 26, 2020
 * @author zal
 */

#include <odGl/GeometryImpl.h>

#include <odGl/GL.h>
#include <odGl/Constants.h>

namespace odGl
{

    GeometryImpl::GeometryImpl()
    : mVertexArrayObjectId(0)
    , mPositionBufferId(0)
    , mNormalBufferId(0)
    , mTextureCoordBufferId(0)
    , mColorBufferId(0)
    {
        glGenVertexArrays(&mVertexArrayObjectId, 1);
        glBindVertexArray(mVertexArrayObjectId);

        // vertex position buffer
        glGenBuffers(&mPositionBufferId, 1);
        glBindBuffer(GL_ARRAY_BUFFER, mPositionBufferId);
        glVertexAttribPointer(Constants::ATTRIBUTE_POSITION_LOCATION, 4, GL_FLOAT​, false, 0, 0);

        // normal buffer
        glGenBuffers(&mNormalBufferId, 1);
        glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferId);
        glVertexAttribPointer(Constants::ATTRIBUTE_NORMAL_LOCATION, 3, GL_FLOAT​, false, 0, 0);

        // texture coord buffer
        glGenBuffers(&mTextureCoordBufferId, 1);
        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordBufferId);
        glVertexAttribPointer(Constants::ATTRIBUTE_TEXTURECOORD_LOCATION, 2, GL_FLOAT​, false, 0, 0);

        // color buffer
        glGenBuffers(&mColorBufferId, 1);
        glBindBuffer(GL_ARRAY_BUFFER, mColorBufferId);
        glVertexAttribPointer(Constants::ATTRIBUTE_TEXTURECOORD_LOCATION, 4, GL_FLOAT​, false, 0, 0);

    }

    void GeometryImpl::setHasBoneInfo(bool b)
    {
    }

    bool GeometryImpl::hasBoneInfo() const
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> GeometryImpl::getBoneIndexArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> GeometryImpl::getBoneWeightArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> GeometryImpl::getVertexArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> GeometryImpl::getColorArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> GeometryImpl::getNormalArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<glm::vec2>> GeometryImpl::getTextureCoordArrayAccessHandler()
    {
    }

    std::unique_ptr<odRender::ArrayAccessHandler<int32_t>> GeometryImpl::getIndexArrayAccessHandler()
    {
    }

    void GeometryImpl::setTexture(odRender::Texture *texture)
    {
    }

    bool GeometryImpl::usesIndexedRendering()
    {
    }

    odRender::PrimitiveType GeometryImpl::getPrimitiveType()
    {
    }


}


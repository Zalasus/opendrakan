/*
 * GuiQuad.cpp
 *
 *  Created on: 14 Dec 2018
 *      Author: zal
 */

#include <odOsg/render/GuiQuad.h>

#include <odCore/Exception.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Utils.h>
#include <odOsg/render/Texture.h>
#include <odOsg/render/Image.h>

namespace odOsg
{

    GuiQuad::GuiQuad()
    : mGeometry(new osg::Geometry)
    , mVertexArray(new osg::Vec3Array(4))
    , mTextureCoordArray(new osg::Vec2Array(4))
    , mColorArray(new osg::Vec4Array(4))
    {
        // vertex order is top-left, bottom-left, bottom-right, top-right
        //  this allows us to use glDrawArrays using a triangle fan, minimizing memory usage
        mGeometry->setVertexArray(mVertexArray);
        mGeometry->setTexCoordArray(0, mTextureCoordArray, osg::Array::BIND_PER_VERTEX);
        mGeometry->setColorArray(mColorArray, osg::Array::BIND_PER_VERTEX);
        mGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));

        setVertexCoords(glm::vec2(0, 0), glm::vec2(1, 1));
        setTextureCoords(glm::vec2(0, 1), glm::vec2(1, 0));

        mColorArray->at(0) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
        mColorArray->at(1) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
        mColorArray->at(2) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
        mColorArray->at(3) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
    }

    GuiQuad::~GuiQuad()
    {
    }

    void GuiQuad::setTexture(odRender::Texture *texture)
    {
        if(texture == nullptr)
        {
            mTexture = nullptr;
            return;
        }

        mTexture = upcast<Texture>(texture);

        mGeometry->getOrCreateStateSet()->setTextureAttribute(0, mTexture->getOsgTexture(), osg::StateAttribute::ON);
    }

    void GuiQuad::setTextureCoords(const glm::vec2 &tl, const glm::vec2 &br)
    {
        mTextureCoordArray->at(0).set(tl.x, tl.y);
        mTextureCoordArray->at(1).set(tl.x, br.y);
        mTextureCoordArray->at(2).set(br.x, br.y);
        mTextureCoordArray->at(3).set(br.x, tl.y);
    }

    void GuiQuad::setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
    {
        glm::vec2 textureDims = (mTexture != nullptr) ? mTexture->getImage()->getDimensionsUV() : glm::vec2(1.0);
        glm::vec2 halfPixelOffset(0.5, 0.5);
        glm::vec2 tlNorm = (topLeft + halfPixelOffset) / textureDims;
        glm::vec2 brNorm = (bottomRight + halfPixelOffset) / textureDims;
        tlNorm.y = 1.0 - tlNorm.y;
        brNorm.y = 1.0 - brNorm.y;

        mTextureCoordArray->at(0).set(tlNorm.x, tlNorm.y);
        mTextureCoordArray->at(1).set(tlNorm.x, brNorm.y);
        mTextureCoordArray->at(2).set(brNorm.x, brNorm.y);
        mTextureCoordArray->at(3).set(brNorm.x, tlNorm.y);
    }

    void GuiQuad::setVertexCoords(const glm::vec2 &tl, const glm::vec2 &br)
    {
        mVertexArray->at(0).set(tl.x, tl.y, 0.0);
        mVertexArray->at(1).set(tl.x, br.y, 0.0);
        mVertexArray->at(2).set(br.x, br.y, 0.0);
        mVertexArray->at(3).set(br.x, tl.y, 0.0);
    }

    void GuiQuad::setColor(const glm::vec4 &color)
    {
        osg::Vec4 c = GlmAdapter::toOsg(color);
        mColorArray->at(0) = c;
        mColorArray->at(1) = c;
        mColorArray->at(2) = c;
        mColorArray->at(3) = c;
    }

}

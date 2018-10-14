/*
 * TexturedQuad.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#include <odCore/gui/TexturedQuad.h>

#include <osg/Array>

namespace odGui
{

    TexturedQuad::TexturedQuad()
    : mVertexArray(new osg::Vec3Array(4))
    , mTextureCoordArray(new osg::Vec2Array(4))
    , mColorArray(new osg::Vec4Array(4))
    , mTexture(nullptr)
    , mZCoord(0.0)
    {
        this->setVertexArray(mVertexArray);

        for(size_t i = 0; i < 4; ++i)
        {
            mColorArray->at(i) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
        }
        this->setColorArray(mColorArray, osg::Array::BIND_PER_VERTEX);

        this->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4, 0));

        this->setDataVariance(osg::Object::DYNAMIC);
        this->setUseDisplayList(false);
        this->setUseVertexBufferObjects(true);

        // set default texture coords spanning whole texture
        setTextureCoords(osg::Vec2(0.0, 1.0), osg::Vec2(1.0, 0.0));
    }

    TexturedQuad::TexturedQuad(const TexturedQuad& quad, const osg::CopyOp &copyop)
    : Geometry(quad, copyop)
    , mZCoord(quad.mZCoord)
    {
        mVertexArray = static_cast<osg::Vec3Array*>(this->getVertexArray());
        mTextureCoordArray = static_cast<osg::Vec2Array*>(this->getTexCoordArray(0));
        mColorArray = static_cast<osg::Vec4Array*>(this->getColorArray());
        mTexture = static_cast<osg::Texture2D*>(this->getOrCreateStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    }

    void TexturedQuad::setTextureImage(odDb::Texture *t)
    {
        if(t == nullptr && mTexture != nullptr)
        {
            this->getOrCreateStateSet()->removeTextureAttribute(0, mTexture);
            mTexture = nullptr;

            this->setTexCoordArray(0, nullptr);

            return;

        }else if(t != nullptr && mTexture == nullptr)
        {
            mTexture = new osg::Texture2D;
            this->getOrCreateStateSet()->setTextureAttributeAndModes(0, mTexture);

            this->setTexCoordArray(0, mTextureCoordArray, osg::Array::BIND_PER_VERTEX);
        }

        mTexture->setImage(t);
    }

    void TexturedQuad::setTextureCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight)
    {
        mTextureCoordArray->at(0) = osg::Vec2(bottomRight.x(), topLeft.y());
        mTextureCoordArray->at(1) = topLeft;
        mTextureCoordArray->at(2) = osg::Vec2(topLeft.x(), bottomRight.y());
        mTextureCoordArray->at(3) = bottomRight;
    }

    void TexturedQuad::setTextureCoordsFromPixels(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight)
    {
        if(mTexture->getImage() == nullptr)
        {
            return;
        }

        osg::Vec2 textureDimensions(mTexture->getImage()->s(), mTexture->getImage()->t());

        osg::Vec2 tl(topLeft.x() + 0.5, topLeft.y() + 0.5);
        tl = osg::componentDivide(tl, textureDimensions);
        tl.y() = 1.0 - tl.y();

        osg::Vec2 br(bottomRight.x() + 0.5, bottomRight.y() + 0.5);
        br = osg::componentDivide(br, textureDimensions);
        br.y() = 1.0 - br.y();

        setTextureCoords(tl, br);
    }

    void TexturedQuad::setVertexCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight)
    {
        // triangle fan: b a c d

        mVertexArray->at(0) = osg::Vec3(bottomRight.x(), topLeft.y(), mZCoord);
        mVertexArray->at(1) = osg::Vec3(topLeft, mZCoord);
        mVertexArray->at(2) = osg::Vec3(topLeft.x(), bottomRight.y(), mZCoord);
        mVertexArray->at(3) = osg::Vec3(bottomRight, mZCoord);

        this->dirtyBound();
    }

    void TexturedQuad::setZCoord(float z)
    {
        mZCoord = z;

        for(size_t i = 0; i < 4; ++i)
        {
            mVertexArray->at(i).z() = z;
        }

        this->dirtyBound();
    }

    void TexturedQuad::setColor(const osg::Vec4 &color)
    {
        for(size_t i = 0; i < 4; ++i)
        {
            mColorArray->at(i) = color;
        }
    }


}

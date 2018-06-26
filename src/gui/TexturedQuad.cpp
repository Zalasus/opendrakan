/*
 * TexturedQuad.cpp
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#include "gui/TexturedQuad.h"

#include <osg/Array>

namespace od
{

    TexturedQuad::TexturedQuad()
    : mVertexArray(new osg::Vec3Array(4))
    , mTextureCoordArray(new osg::Vec2Array(4))
    , mColorArray(new osg::Vec4Array(4))
    , mTexture(new osg::Texture2D)
    , mZCoord(1)
    {
        this->setVertexArray(mVertexArray);
        this->setTexCoordArray(0, mTextureCoordArray, osg::Array::BIND_PER_VERTEX);

        for(size_t i = 0; i < 4; ++i)
        {
            mColorArray->at(i) = osg::Vec4(1.0, 1.0, 1.0, 1.0);
        }
        this->setColorArray(mColorArray, osg::Array::BIND_PER_VERTEX);

        GLubyte elements[] = { 0, 2, 1, 1, 2, 3 };
        mDrawElements = new osg::DrawElementsUByte(GL_TRIANGLES, 6, elements);
        this->addPrimitiveSet(mDrawElements);

        this->getOrCreateStateSet()->setTextureAttributeAndModes(0, mTexture);

        this->setDataVariance(osg::Object::DYNAMIC);
        this->setUseDisplayList(false);
        this->setUseVertexBufferObjects(true);
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

    void TexturedQuad::setTextureCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight)
    {
        mTextureCoordArray->at(0) = topLeft;
        mTextureCoordArray->at(1) = osg::Vec2(bottomRight.x(), topLeft.y());
        mTextureCoordArray->at(2) = osg::Vec2(topLeft.x(), bottomRight.y());
        mTextureCoordArray->at(3) = bottomRight;
    }

    void TexturedQuad::setTexture(Texture *texture)
    {
        mTextureAsset = texture;
        mTexture->setImage(texture);
    }

    void TexturedQuad::setVertexCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight)
    {
        mVertexArray->at(0) = osg::Vec3(topLeft, mZCoord);
        mVertexArray->at(1) = osg::Vec3(bottomRight.x(), topLeft.y(), mZCoord);
        mVertexArray->at(2) = osg::Vec3(topLeft.x(), bottomRight.y(), mZCoord);
        mVertexArray->at(3) = osg::Vec3(bottomRight, mZCoord);

        this->dirtyBound();
    }

    void TexturedQuad::setZCoord(float z)
    {
        mZCoord = z;

        mVertexArray->at(0).z() = z;
        mVertexArray->at(1).z() = z;
        mVertexArray->at(2).z() = z;
        mVertexArray->at(3).z() = z;

        this->dirtyBound();
    }


}

/*
 * GuiQuad.h
 *
 *  Created on: 14 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_GUIQUAD_H_
#define INCLUDE_ODOSG_GUIQUAD_H_

#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>

#include <odCore/render/GuiQuad.h>

namespace odOsg
{
    class Texture;

    class GuiQuad : public odRender::GuiQuad
    {
    public:

        GuiQuad(osg::Group *guiRoot);
        virtual ~GuiQuad();

        virtual void setMatrix(const glm::mat4 &m) override;

        virtual void setTexture(odRender::Texture *texture) override;
        virtual void setTextureCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) override;
        virtual void setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) override;
        virtual void setVertexCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight) override;


    private:

        osg::ref_ptr<osg::Group> mGuiRoot;
        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Geode> mGeode;
        osg::ref_ptr<osg::Geometry> mGeometry;
        osg::ref_ptr<osg::Vec3Array> mVertexArray;
        osg::ref_ptr<osg::Vec2Array> mTextureCoordArray;
        osg::ref_ptr<osg::Vec4Array> mColorArray;

        od::RefPtr<Texture> mTexture;
    };

}

#endif /* INCLUDE_ODOSG_GUIQUAD_H_ */

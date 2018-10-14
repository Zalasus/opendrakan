/*
 * TexturedQuad.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_TEXTUREDQUAD_H_
#define INCLUDE_GUI_TEXTUREDQUAD_H_

#include <osg/Geometry>

#include <odCore/db/Texture.h>

namespace odGui
{

    /**
     * @brief Dynamic, flat, textured quad for GUI stuff.
     *
     * This is a osg::Geometry for representing a rectangle defined by it's top left and bottom right corners.
     * It is always parallel to the xy-plane. The z component can be specified, but it is shared by all four vertices.
     */
    class TexturedQuad : public osg::Geometry
    {
    public:

        TexturedQuad();
        TexturedQuad(const TexturedQuad& quad, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

        // note: Even though Geometry is now a osg::Node, in the version of osg available on Ubuntu Trusty,
        //  it still isn't. So don't use META_Node here unless you want to break our CI.
        META_Object(od, TexturedQuad);

        /**
         * @brief Return osg::Texture object used. Might return nullptr if no texture has been set yet.
         */
        inline osg::Texture2D *getTexture() { return mTexture; }

        /**
         * @brief Set texture image to be used. Pass nullptr to disable texturing for this quad.
         */
        void setTextureImage(odDb::Texture *t);

        void setTextureCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight);
        /**
         * @brief Sets texture coordinates from image pixel coordinates.
         *
         * This will set the texture coordinates to be placed exactly at the specified pixels (including half pixel offset).
         * The coordinate origin is at the top left corner, as is common in image processing.
         * For this to work, an image asset must have been assigned to this TexturedQuad. Otherwise, the call will be ignored.
         */
        void setTextureCoordsFromPixels(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight);
        void setVertexCoords(const osg::Vec2 &topLeft, const osg::Vec2 &bottomRight);
        void setZCoord(float z);
        void setColor(const osg::Vec4 &color);


    private:

        osg::ref_ptr<osg::Vec3Array> mVertexArray;
        osg::ref_ptr<osg::Vec2Array> mTextureCoordArray;
        osg::ref_ptr<osg::Vec4Array> mColorArray;
        osg::ref_ptr<osg::Texture2D> mTexture;
        float mZCoord;
    };

}

#endif /* INCLUDE_GUI_TEXTUREDQUAD_H_ */

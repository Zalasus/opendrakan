/**
 * @file GeometryImpl.h
 * @date Jan 26, 2020
 * @author zal
 */

#ifndef INCLUDE_ODGL_GEOMETRYIMPL_H_
#define INCLUDE_ODGL_GEOMETRYIMPL_H_

#include <odGl/GL.h>

#include <odCore/render/Geometry.h>

namespace odGl
{

    class GeometryImpl : public odRender::Geometry
    {
    public:

        GeometryImpl();

        virtual void setHasBoneInfo(bool b) override;
        virtual bool hasBoneInfo() const override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getBoneIndexArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getBoneWeightArrayAccessHandler() override;

        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> getVertexArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec4>> getColorArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec3>> getNormalArrayAccessHandler() override;
        virtual std::unique_ptr<odRender::ArrayAccessHandler<glm::vec2>> getTextureCoordArrayAccessHandler() override;

        virtual std::unique_ptr<odRender::ArrayAccessHandler<int32_t>> getIndexArrayAccessHandler() override;

        virtual void setTexture(odRender::Texture *texture) override;

        virtual bool usesIndexedRendering() override;
        virtual odRender::PrimitiveType getPrimitiveType() override;


    private:

        GLuint mVertexArrayObjectId;

        GLuint mPositionBufferId;
        GLuint mNormalBufferId;
        GLuint mTextureCoordBufferId;
        GLuint mColorBufferId;

    };

}

#endif /* INCLUDE_ODGL_GEOMETRYIMPL_H_ */

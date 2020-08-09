
#include <odCore/gui/Quad.h>

#include <odCore/Exception.h>

#include <odCore/db/AssetProvider.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Handle.h>
#include <odCore/render/Model.h>
#include <odCore/render/Texture.h>
#include <odCore/render/Image.h>
#include <odCore/render/Geometry.h>

namespace odGui
{
    Quad::Quad()
    {
    }

    Quad::Quad(odRender::Renderer &renderer)
    {
        init(renderer);
    }

    void Quad::init(odRender::Renderer &renderer)
    {
        if(!empty()) return;

        mHandle = renderer.createHandle(odRender::RenderSpace::NONE);

        mModel = renderer.createModel();
        mHandle->setModel(mModel);

        // vertex order is top-left, bottom-left, bottom-right, top-right
        //  this allows us to use glDrawArrays using a triangle fan, minimizing memory usage
        mGeometry = renderer.createGeometry(odRender::PrimitiveType::TRIANGLE_FAN, false);
        mModel->addGeometry(mGeometry);

        odRender::ArrayAccessor<glm::vec3>(mGeometry->getVertexArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE).resize(4);
        odRender::ArrayAccessor<glm::vec2>(mGeometry->getTextureCoordArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE).resize(4);
        odRender::ArrayAccessor<glm::vec4>(mGeometry->getColorArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE).resize(4);
    }

    void Quad::setTexture(std::shared_ptr<odRender::Texture> texture)
    {
        _check();

        mTexture = texture;
        mGeometry->setTexture(texture);
    }

    void Quad::setTextureFromDb(odDb::AssetProvider &ap, const odDb::AssetRef &textureRef, odRender::Renderer &renderer)
    {
        _check();

        std::shared_ptr<odDb::Texture> dbTexture = ap.getAssetByRef<odDb::Texture>(textureRef);
        std::shared_ptr<odRender::Image> image = renderer.getOrCreateImageFromDb(dbTexture);
        std::shared_ptr<odRender::Texture> texture = renderer.createTexture(image, odRender::TextureReuseSlot::NONE);
        this->setTexture(texture);
    }

    void Quad::setTextureFromDb(odDb::AssetProvider &ap, const od::RecordId &textureId, odRender::Renderer &renderer)
    {
        _check();

        setTextureFromDb(ap, odDb::AssetRef(textureId, 0), renderer);
    }

    void Quad::setTextureCoords(const glm::vec2 &tl, const glm::vec2 &br)
    {
        _check();

        odRender::ArrayAccessor<glm::vec2> uvArray(mGeometry->getTextureCoordArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
        uvArray[0] = { tl.x, tl.y };
        uvArray[1] = { tl.x, br.y };
        uvArray[2] = { br.x, br.y };
        uvArray[3] = { br.x, tl.y };
    }

    void Quad::setTextureCoordsFromPixels(const glm::vec2 &topLeft, const glm::vec2 &bottomRight)
    {
        _check();

        glm::vec2 textureDims = (mTexture != nullptr) ? mTexture->getImage()->getDimensionsUV() : glm::vec2(1.0);
        glm::vec2 halfPixelOffset(0.5, 0.5);
        glm::vec2 tlNorm = (topLeft + halfPixelOffset) / textureDims;
        glm::vec2 brNorm = (bottomRight + halfPixelOffset) / textureDims;
        tlNorm.y = 1.0 - tlNorm.y;
        brNorm.y = 1.0 - brNorm.y;

        odRender::ArrayAccessor<glm::vec2> uvArray(mGeometry->getTextureCoordArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
        uvArray[0] = { tlNorm.x, tlNorm.y };
        uvArray[1] = { tlNorm.x, brNorm.y };
        uvArray[2] = { brNorm.x, brNorm.y };
        uvArray[3] = { brNorm.x, tlNorm.y };
    }

    void Quad::setVertexCoords(const glm::vec2 &tl, const glm::vec2 &br)
    {
        _check();

        odRender::ArrayAccessor<glm::vec3> vertexArray(mGeometry->getVertexArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
        vertexArray[0] = { tl.x, tl.y, 0.0 };
        vertexArray[1] = { tl.x, br.y, 0.0 };
        vertexArray[2] = { br.x, br.y, 0.0 };
        vertexArray[3] = { br.x, tl.y, 0.0 };
    }

    void Quad::setColor(const glm::vec4 &color)
    {
        _check();

        odRender::ArrayAccessor<glm::vec4> colorArray(mGeometry->getColorArrayAccessHandler(), odRender::ArrayAccessMode::REPLACE);
        colorArray[0] = color;
        colorArray[1] = color;
        colorArray[2] = color;
        colorArray[3] = color;
    }

    void Quad::_check()
    {
        if(empty())
        {
            throw od::Exception("Quad is empty");
        }
    }

}

/*
 * GuiQuad.cpp
 *
 *  Created on: 23 Dec 2018
 *      Author: zal
 */

#include <odCore/render/GuiQuad.h>

#include <odCore/db/AssetProvider.h>
#include <odCore/db/Texture.h>

#include <odCore/render/Image.h>
#include <odCore/render/Texture.h>

namespace odRender
{

    void GuiQuad::setTextureFromDb(odDb::AssetProvider &ap, const odDb::AssetRef &textureRef, odRender::Renderer &renderer)
    {
        od::RefPtr<odDb::Texture> dbTexture = ap.getAssetByRef<odDb::Texture>(textureRef);
        od::RefPtr<odRender::Texture> texture = dbTexture->getRenderImage(&renderer)->createTexture();
        this->setTexture(texture);
    }

}

/*
 * Texture.h
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <odCore/SrscFile.h>
#include <odCore/WeakRefPtr.h>

#include <odCore/db/Asset.h>
#include <odCore/db/Class.h>

namespace odRender
{
    class Image;
    class Texture;
}

namespace odDb
{

	class TextureFactory;

    class Texture : public Asset
    {
    public:

	    enum class Usage
	    {
	        Model,
	        Layer,
	        Custom
	    };

        Texture(AssetProvider &ap, od::RecordId id);
        ~Texture();

        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }
        inline uint8_t *getRawR8G8B8A8Data() { return mRgba8888Data.data(); }
        inline bool hasAlpha() const { return mHasAlphaChannel; };

        void loadFromRecord(TextureFactory &factory, od::DataReader dr);
        void exportToPng(const od::FilePath &path);

        od::RefPtr<odRender::Image> getRenderImage(odRender::Renderer *renderer);

        /**
         * Provides an odRender::Texture object that can be used in the rendering system.
         *
         * Pixel storage is shared between both this object and all created render textures.
         * The odRender::Texture object itself only represents the texture's rendering parameters
         * like filtering, wrapping etc.
         *
         * If an odRender::Texture with a usage other than Usage::Custom has already been aquired,
         * the same object will be returned, as they can be shared among using models/layers.
         *
         * Textures aquired via this method with usage = Usage::Custom are always unique.
         */
        od::RefPtr<odRender::Texture> getRenderTexture(odRender::Renderer *renderer, Usage usage);


    private:

        unsigned char _filter16BitChannel(uint16_t color, uint16_t mask);

        uint32_t mWidth;
        uint32_t mHeight;
        uint16_t mBitsPerPixel;
        uint16_t mAlphaBitsPerPixel;
        uint32_t mColorKey;
        od::RecordId mMipMapId;
        od::RecordId mAlternateId;
        uint16_t mFlags;
        uint16_t mMipMapNumber;
        AssetRef mMaterialClassRef;
        uint32_t mUsageCount;
        uint32_t mCompressionLevel;
        uint32_t mCompressedSize;

        bool mHasAlphaChannel;
        AssetPtr<Class> mMaterialClass;
        std::unique_ptr<odRfl::RflClass> mMaterialInstance;

        std::vector<uint8_t> mRgba8888Data;

        od::WeakRefPtr<odRender::Image> mRenderImage;
        od::WeakRefPtr<odRender::Texture> mModelRenderTexture;
        od::WeakRefPtr<odRender::Texture> mLayerRenderTexture;
    };

    template <>
    struct AssetTraits<Texture>
    {
        static const char *name()
        {
            return "Texture";
        }
    };

}




#endif /* TEXTURE_H_ */

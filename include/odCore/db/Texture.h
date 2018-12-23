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

        Texture(AssetProvider &ap, od::RecordId id);
        ~Texture();

        inline uint32_t getWidth() const { return mWidth; }
        inline uint32_t getHeight() const { return mHeight; }
        inline uint8_t *getRawR8G8B8A8Data() { return mRgba8888Data.data(); }
        inline bool hasAlpha() const { return mHasAlphaChannel; };

        void loadFromRecord(TextureFactory &factory, od::DataReader dr);
        void exportToPng(const od::FilePath &path);

        /**
         * Provides an odRender::Image object that can be used to create Texture objects.
         *
         * An Image encapsulates pixel data storage, so pixel data may be shared between multiple textures.
         */
        od::RefPtr<odRender::Image> getRenderImage(odRender::Renderer *renderer);


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
        od::RefPtr<Class> mMaterialClass;
        std::unique_ptr<odRfl::RflClass> mMaterialInstance;

        std::vector<uint8_t> mRgba8888Data;

        od::WeakRefPtr<odRender::Image> mRenderImage;
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

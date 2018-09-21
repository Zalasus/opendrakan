/*
 * Texture.h
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <osg/Image>

#include <odCore/SrscFile.h>
#include <odCore/db/Asset.h>
#include <odCore/db/Class.h>

namespace od
{

	class TextureFactory;

    class Texture : public Asset, public osg::Image
    {
    public:

        Texture(AssetProvider &ap, RecordId id);

        inline bool hasAlpha() const { return mHasAlphaChannel; };

        void loadFromRecord(TextureFactory &factory, DataReader dr);
        void exportToPng(const FilePath &path);

        // override osg::Image
        virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "Texture"; }


    private:

        unsigned char _filter16BitChannel(uint16_t color, uint16_t mask);

        uint32_t mWidth;
        uint32_t mHeight;
        uint16_t mBitsPerPixel;
        uint16_t mAlphaBitsPerPixel;
        uint32_t mColorKey;
        RecordId mMipMapId;
        RecordId mAlternateId;
        uint16_t mFlags;
        uint16_t mMipMapNumber;
        AssetRef mMaterialClassRef;
        uint32_t mUsageCount;
        uint32_t mCompressionLevel;
        uint32_t mCompressedSize;

        bool mHasAlphaChannel;
        osg::ref_ptr<Class> mMaterialClass;
        std::unique_ptr<RflClass> mMaterialInstance;
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

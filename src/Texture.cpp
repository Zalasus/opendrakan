/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include "Texture.h"

#include "ZStream.h"

namespace od
{

    Texture::Texture()
    : mWidth(0)
    , mHeight(0)
    , mBitsPerPixel(0)
    , mAlphaBitsPerPixel(0)
    , mImage(new osg::Image)
    {

    }

    void Texture::loadFromRecord(SrscFile &srscFile, RecordId id)
    {
        DataReader dr(srscFile.getStreamForRecordTypeId(0x0040, id));

        uint32_t rowSpacing;

        dr >> mWidth
           >> mHeight
           >> rowSpacing
           >> mBitsPerPixel
           >> mAlphaBitsPerPixel;

        dr.ignore(32);

        size_t bytePerPixel;
        switch(mBitsPerPixel)
        {
        case 8:
            throw Exception("Can't load 8bpp textured right now as I can't be bothered to read the palette");

        case 16:
            bytePerPixel = 2;
            break;

        case 24:
            bytePerPixel = 3;
            break;

        case 32:
            bytePerPixel = 4;
            break;

        default:
            throw Exception("Invalid BPP");
        }

        uint32_t compressionLevel;
        uint32_t compressedSize;

        dr >> compressionLevel
           >> compressedSize;

        if(compressionLevel == 0)
        {
            throw Exception("Can't handle uncompressed textures right now");
        }

        ZStream zstr(dr.getStream());

        std::vector<char> pixBuffer(mWidth*mHeight*bytePerPixel);
        zstr.read(pixBuffer.data(), pixBuffer.size());

        //mImage->setImage(mWidth, mHeight, 0, 0xdeadbeef, 0, 0, pixBuffer.data(), osg::Image::AllocationMode::USE_NEW_DELETE, 1, mWidth);

    }

    OD_REGISTER_ASSET_TYPE(ASSET_TEXTURE, Texture, "txd");

}

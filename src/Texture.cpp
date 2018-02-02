/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include "Texture.h"

#include "ZStream.h"
#include "Logger.h"

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
    	Logger::info() << "Loading texture " << id;

        DataReader dr(srscFile.getStreamForRecordTypeId(0x0040, id));

        uint32_t rowSpacing;

        dr >> mWidth
           >> mHeight
           >> rowSpacing
           >> mBitsPerPixel
           >> mAlphaBitsPerPixel;

        dr.ignore(30);

        size_t bytePerPixel;
        GLint pixelType;
        switch(mBitsPerPixel)
        {
        case 8:
            throw Exception("Can't load 8bpp textured right now as I can't be bothered to read the palette");

        case 16:
            bytePerPixel = 2;
            pixelType = GL_UNSIGNED_SHORT_5_6_5; // unless alpha!
            break;

        case 24:
            bytePerPixel = 3;
            pixelType = GL_3_BYTES;
            break;

        case 32:
            bytePerPixel = 4;
            pixelType = GL_UNSIGNED_INT_8_8_8_8; // FIXME: i don't think this really is the right format
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
        zstr.seekToEndOfZlib();

        unsigned char *pixData = reinterpret_cast<unsigned char*>(pixBuffer.data());
        mImage->setImage(mWidth, mHeight, 0, GL_RGB, GL_RGB, pixelType, pixData, osg::Image::USE_NEW_DELETE);

        Logger::info() << "Texture successfully loaded";
    }

    OD_REGISTER_ASSET_TYPE(ASSET_TEXTURE, Texture, "txd");

}

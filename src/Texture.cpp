/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include "Texture.h"

#include <functional>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include "ZStream.h"
#include "Logger.h"
#include "Exception.h"
#include "TextureFactory.h"

#define OD_TEX_FLAG_HIGHQUALITY         0x0080
#define OD_TEX_FLAG_DYNAMICTEXTURE      0x0040
#define OD_TEX_FLAG_NEXTFRAME           0x0020
#define OD_TEX_FLAG_MIPMAP              0x0008
#define OD_TEX_FLAG_ALTERNATE           0x0004
#define OD_TEX_FLAG_ALPHACHANNEL        0x0002
#define OD_TEX_FLAG_ALPHAMAP            0x0001

namespace od
{

    Texture::Texture(RecordId id)
    : mId(id)
    , mWidth(0)
    , mHeight(0)
    , mBitsPerPixel(0)
    , mAlphaBitsPerPixel(0)
    , mColorKey(0)
    , mMipMapId(0)
    , mAlternateId(0)
    , mFlags(0)
    , mMipMapNumber(0)
    , mUsageCount(0)
    , mCompressionLevel(0)
    , mCompressedSize(0)
    {

    }

    void Texture::loadFromRecord(TextureFactory &factory, SrscFile &srscFile, SrscFile::DirIterator record)
    {
    	Logger::verbose() << "Loading texture " << std::hex << mId << std::dec;

        DataReader dr(srscFile.getStreamForRecord(record));

        uint32_t rowSpacing;

        dr >> mWidth
           >> mHeight
           >> rowSpacing
           >> mBitsPerPixel
           >> mAlphaBitsPerPixel
           >> DataReader::Ignore(2)
           >> mColorKey
           >> mMipMapId
           >> DataReader::Ignore(2)
           >> mAlternateId
           >> DataReader::Ignore(6)
           >> mFlags
           >> mMipMapNumber
           >> mClassRef
           >> mUsageCount
           >> mCompressionLevel
           >> mCompressedSize;

        if(mFlags & OD_TEX_FLAG_ALPHAMAP)
        {
        	Logger::error() << "Unsupported alpha map with " << mAlphaBitsPerPixel << "BPP";
        	throw UnsupportedException("Alpha maps unsupported right now");
        }

        size_t bytePerPixel;
        switch(mBitsPerPixel)
        {
        case 8:
            bytePerPixel = 1;
            break;

        case 16:
            bytePerPixel = 2;
            switch(mAlphaBitsPerPixel)
            {
            case 0:
            case 1:
            case 4:
            case 8:
                break;

            default:
                throw Exception("Invalid alpha BPP count");
            }
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

        if(mCompressionLevel == 0)
        {
            throw UnsupportedException("Can't handle uncompressed textures right now");
        }

        ZStream zstr(dr.getStream());
        DataReader zdr(zstr);

        // TODO: this may be more efficient, but it's still messy as hell
        std::function<void(unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)> pixelReaderFunc;
        if(bytePerPixel == 1)
        {
            pixelReaderFunc = [&zdr, &factory](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                uint8_t palIndex;
                zdr >> palIndex;

                TextureFactory::PaletteColor palColor = factory.getPaletteColor(palIndex);

                red = palColor.red;
                green = palColor.green;
                blue = palColor.blue;
                alpha = 0xff;
            };

        }else if(bytePerPixel == 2)
        {
            /*
             * ABPP    R:G:B+A bits
             *  1       5:5:5+1      RRRRRGGG GGBBBBBA
             *  4       4:4:4+4      RRRRGGGG BBBBAAAA
             *  8       3:3:2+8      RRRGGGBB AAAAAAAA
             */
            if(mAlphaBitsPerPixel == 0 || !(mFlags & OD_TEX_FLAG_ALPHACHANNEL))
            {
                pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
                {
                    uint16_t color;
                    zdr >> color;

                    red   = (color >> 8) & 0xf8;
                    green = (color >> 3) & 0xfc;
                    blue  = (color << 3) & 0xf8;
                    alpha = 0xff;
                };

            }else if(mAlphaBitsPerPixel == 1)
            {
                pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
                {
                    uint16_t color;
                    zdr >> color;

                    red   = (color >> 8) & 0xf8;
                    green = (color >> 3) & 0xf8;
                    blue  = (color << 2) & 0xf8;
                    alpha = (color << 7) & 0x80;
                };

            }else if(mAlphaBitsPerPixel == 4)
            {
                pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
                {
                    uint16_t color;
                    zdr >> color;

                    red   = (color >> 8) & 0xf0;
                    green = (color >> 4) & 0xf0;
                    blue  = color & 0xf0;
                    alpha = (color << 4) & 0xf0;
                };

            }else if(mAlphaBitsPerPixel == 8)
            {
                pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
                {
                    uint16_t color;
                    zdr >> color;

                    red   = (color >> 8) & 0xe0;
                    green = (color >> 5) & 0xe0;
                    blue  = (color >> 2) & 0xc0;
                    alpha = color & 0xff;
                };
            }

        }else if(bytePerPixel == 3)
        {
            pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                zdr >> red
                    >> green
                    >> blue;

                alpha = 0xff;
            };

        }else if(bytePerPixel == 4)
        {
            pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                // i know, this format makes little sense
                zdr >> green
                    >> blue
                    >> DataReader::Ignore(1)
                    >> red;

                alpha = 0xff;
            };
        }

        // translate whatever is stored in texture into 32-bit RGBA format
        unsigned char *pixBuffer = new unsigned char[mWidth*mHeight*4]; // no need for RAII, osg takes ownership
        for(size_t i = 0; i < mWidth*mHeight*3; i += 3)
        {
            pixelReaderFunc(pixBuffer[i], pixBuffer[i+1], pixBuffer[i+2], pixBuffer[i+3]);
        }
        zstr.seekToEndOfZlib();

        this->setImage(mWidth, mHeight, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, pixBuffer, osg::Image::USE_NEW_DELETE);

        Logger::verbose() << "Texture successfully loaded";
    }

    void Texture::exportToPng(const FilePath &path)
    {
		osgDB::writeImageFile(*this, path.str());
    }

}

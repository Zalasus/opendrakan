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

#define OD_TEX_OPAQUE_ALPHA 			0xff

namespace od
{

    Texture::Texture(RecordId id)
    : Asset(id)
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

    void Texture::loadFromRecord(TextureFactory &factory, DataReader dr)
    {
    	Logger::verbose() << "Loading texture " << std::hex << this->getAssetId() << std::dec;

        uint32_t rowSpacing; // TODO: shouldn't we do something with this?

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

        if(mCompressionLevel == 0)
        {
            throw UnsupportedException("Can't handle uncompressed textures right now");
        }

        ZStream zstr(dr.getStream());
        DataReader zdr(zstr);

        std::function<void(unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)> pixelReaderFunc;
        if(mBitsPerPixel == 8)
        {
            pixelReaderFunc = [&zdr, &factory](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                uint8_t palIndex;
                zdr >> palIndex;

                TextureFactory::PaletteColor palColor = factory.getPaletteColor(palIndex);

                red = palColor.red;
                green = palColor.green;
                blue = palColor.blue;
                alpha = OD_TEX_OPAQUE_ALPHA;
            };

        }else if(mBitsPerPixel == 16)
        {
            /*
             * ABPP    R:G:B+A bits
             *  0       5:6:5+0      RRRRRGGG GGGBBBBB
             *  1       5:5:5+1      RRRRRGGG GGBBBBBA
             *  4       4:4:4+4      RRRRGGGG BBBBAAAA
             *  8       3:3:2+8      RRRGGGBB AAAAAAAA
             */

        	uint32_t redMask;
			uint32_t greenMask;
			uint32_t blueMask;
			uint32_t alphaMask;

            if(mAlphaBitsPerPixel == 0 || !(mFlags & OD_TEX_FLAG_ALPHACHANNEL))
            {
                redMask =   0xf800;
                greenMask = 0x07e0;
                blueMask =  0x001f;
                alphaMask = 0;

            }else if(mAlphaBitsPerPixel == 1)
            {
                redMask =   0xf800;
                greenMask = 0x07c0;
                blueMask =  0x003e;
                alphaMask = 0x0001;

            }else if(mAlphaBitsPerPixel == 4)
            {
                redMask =   0xf000;
                greenMask = 0x0f00;
                blueMask =  0x00f0;
                alphaMask = 0x000f;

            }else if(mAlphaBitsPerPixel == 8)
            {
                redMask =   0xe000;
                greenMask = 0x1c00;
                blueMask =  0x0300;
                alphaMask = 0x00ff;

            }else
            {
            	throw Exception("Invalid alpha BPP count");
            }

            pixelReaderFunc = [&zdr, redMask, greenMask, blueMask, alphaMask](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
            	uint16_t c;
            	zdr >> c;

            	red = 0xff * ((c & redMask)/static_cast<float>(redMask));
            	green = 0xff * ((c & greenMask)/static_cast<float>(greenMask));
            	blue = 0xff * ((c & blueMask)/static_cast<float>(blueMask));
            	alpha = alphaMask ? (0xff * ((c & alphaMask)/static_cast<float>(alphaMask))) : OD_TEX_OPAQUE_ALPHA;
            };

        }else if(mBitsPerPixel == 24)
        {
            pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                zdr >> red
                    >> green
                    >> blue;

                alpha = OD_TEX_OPAQUE_ALPHA;
            };

        }else if(mBitsPerPixel == 32)
        {
            pixelReaderFunc = [&zdr](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                // i know, this format makes little sense
                zdr >> green
                    >> blue
                    >> DataReader::Ignore(1)
                    >> red;

                alpha = OD_TEX_OPAQUE_ALPHA;
            };

        }else
        {
        	throw Exception("Invalid BPP");
        }

        // translate whatever is stored in texture into 32-bit RGBA format
        unsigned char *pixBuffer = new unsigned char[mWidth*mHeight*4]; // no need for RAII, osg takes ownership
        for(size_t i = 0; i < mWidth*mHeight*4; i += 4)
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

/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include "db/Texture.h"

#include <functional>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include "ZStream.h"
#include "Logger.h"
#include "Exception.h"
#include "db/TextureFactory.h"
#include "db/Database.h"
#include "DbManager.h"
#include "rfl/dragon/Material.h"

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

    Texture::Texture(AssetProvider &ap, RecordId id)
    : Asset(ap, id)
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
    , mHasAlphaChannel(false)
    {

    }

    void Texture::loadFromRecord(TextureFactory &factory, DataReader dr)
    {
    	Logger::debug() << "Loading texture " << std::hex << this->getAssetId() << std::dec;

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

        if(mCompressionLevel == 0)
        {
            throw UnsupportedException("Can't handle uncompressed textures right now");
        }

        // take apart color key so we can compare it more efficiently
        bool hasColorKey = (mColorKey != 0xffffffff);
        uint8_t keyRed   = (mColorKey & 0xff0000) >> 16;
        uint8_t keyGreen = (mColorKey & 0x00ff00) >> 8;
        uint8_t keyBlue  = (mColorKey & 0x0000ff);

        mHasAlphaChannel = (mAlphaBitsPerPixel != 0) || hasColorKey;

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
             * ABPP    R:G:B+A bits   Bit pattern (LE adjusted!)
             * 0       5:6:5+0        RRRRRGGG GGGBBBBB
             * 1       5:5:5+1        ARRRRRGG GGGBBBBB
             * 4       4:4:4+4        AAAARRRR GGGGBBBB
             * 8       3:3:2+8        AAAAAAAA RRRGGGBB
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
                redMask =   0x7c00;
                greenMask = 0x03e0;
                blueMask =  0x001f;
                alphaMask = 0x8000;

            }else if(mAlphaBitsPerPixel == 4)
            {
                redMask =   0x0f00;
                greenMask = 0x00f0;
                blueMask =  0x000f;
                alphaMask = 0xf000;

            }else if(mAlphaBitsPerPixel == 8)
            {
                redMask =   0x00e0;
                greenMask = 0x001c;
                blueMask =  0x0003;
                alphaMask = 0xff00;

            }else
            {
            	throw Exception("Invalid alpha BPP count");
            }

            pixelReaderFunc = [this, &zdr, redMask, greenMask, blueMask, alphaMask](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
            	uint16_t c;
            	zdr >> c;

            	red = _filter16BitChannel(c, redMask);
            	green = _filter16BitChannel(c, greenMask);
            	blue = _filter16BitChannel(c, blueMask);
            	alpha = alphaMask ? _filter16BitChannel(c, alphaMask) : OD_TEX_OPAQUE_ALPHA;
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

         uint32_t trailingBytes = rowSpacing - mWidth*(mBitsPerPixel/8);
         if(trailingBytes)
         {
        	 throw UnsupportedException("Can only load packed textures right now");
         }

        // translate whatever is stored in texture into 8-bit RGBA format
        unsigned char *pixBuffer = new unsigned char[mWidth*mHeight*4]; // no need for RAII, osg takes ownership
        for(size_t i = 0; i < mWidth*mHeight*4; i += 4)
        {
        	uint8_t red;
        	uint8_t green;
        	uint8_t blue;
        	uint8_t alpha;

            pixelReaderFunc(red, green, blue, alpha);

            if(hasColorKey && red == keyRed && green == keyGreen && blue == keyBlue)
            {
            	alpha = 0;
            }

            pixBuffer[i]   = red;
            pixBuffer[i+1] = green;
            pixBuffer[i+2] = blue;
            pixBuffer[i+3] = alpha;
        }
        zstr.seekToEndOfZlib();



        this->setImage(mWidth, mHeight, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, pixBuffer, osg::Image::USE_NEW_DELETE);

        if(!mClassRef.isNull())
        {
        	mClass = this->getAssetProvider().getClassByRef(mClassRef);
        	std::unique_ptr<odRfl::RflClass> rflClass = mClass->makeInstance();
        	mMaterial = std::unique_ptr<odRfl::Material>(dynamic_cast<odRfl::Material*>(rflClass.release()));
        	mMaterial->loaded(factory.getEngine(), nullptr);
        }

        Logger::debug() << "Texture successfully loaded";
    }

    void Texture::exportToPng(const FilePath &path)
    {
        Logger::verbose() << "Exporting texture " << std::hex << getAssetId() << std::dec
                << " with dimensions " << mWidth << "x" << mHeight
                << " to file '" << path.str() << "'";

		osgDB::writeImageFile(*this, path.str());
    }

    unsigned char Texture::_filter16BitChannel(uint16_t color, uint16_t mask)
    {
    	// filtering algorithm:
    	//    mask out channel
    	//    shift channel so highest channel bit lies in highest output bit
    	//    copy lowest channel bit to all output bits not occupied by channel after shift
    	//
    	//   too complicated to do it with as few instructions as possible. try using float operations

    	//uint16_t channelShifted = (color & mask) << shift;
    	//uint16_t bitToRepeatMask = (mask << shift) & ~(mask << shift + 1);

    	float percentColor = ((float)(color&mask)/mask);

    	return 0xff * percentColor;
    }

}

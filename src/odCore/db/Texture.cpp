/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Texture.h>

#include <functional>

#include <odCore/ZStream.h>
#include <odCore/Logger.h>
#include <odCore/Exception.h>

#include <odCore/db/TextureFactory.h>
#include <odCore/db/Database.h>
#include <odCore/db/DbManager.h>
#include <odCore/db/DependencyTable.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/Image.h>
#include <odCore/render/Texture.h>

#define OD_TEX_FLAG_HIGHQUALITY         0x80
#define OD_TEX_FLAG_DYNAMICTEXTURE      0x40
#define OD_TEX_FLAG_NEXTFRAME           0x20
#define OD_TEX_FLAG_MIPMAP              0x08
#define OD_TEX_FLAG_ALTERNATE           0x04
#define OD_TEX_FLAG_ALPHACHANNEL        0x02
#define OD_TEX_FLAG_ALPHAMAP            0x01

#define OD_TEX_OPAQUE_ALPHA 			0xff

namespace odDb
{

    Texture::Texture(TextureFactory &factory)
    : mTextureFactory(factory)
    , mWidth(0)
    , mHeight(0)
    , mBitsPerPixel(0)
    , mAlphaBitsPerPixel(0)
    , mColorKey(0)
    , mAnimationFps(0)
    , mFlags(0)
    , mMipMapNumber(0)
    , mUsageCount(0)
    , mCompressionLevel(0)
    , mCompressedSize(0)
    , mIsNextFrame(false)
    , mAnimFrameCount(1)
    , mHasAlphaChannel(false)
    {
    }

    Texture::~Texture()
    {
    }

    void Texture::load(od::SrscFile::RecordInputCursor cursor)
    {
        {
            od::DataReader dr = cursor.getReader();
            _loadFromRecord(dr);
        }

        if(!isNextFrame())
        {
            mAnimFrameCount = 1;
            while(cursor.nextOfTypeId(od::SrscRecordType::TEXTURE, getAssetId()+mAnimFrameCount, 1))
            {
                od::DataReader dr = cursor.getReader();
                // skip to flags to check
                uint8_t animFps;
                uint8_t flags;

                dr >> od::DataReader::Ignore(34)
                   >> animFps
                   >> flags;

                if(!(flags & OD_TEX_FLAG_NEXTFRAME))
                {
                    break;
                }

                ++mAnimFrameCount;
            }
        }
    }

    void Texture::postLoad()
    {
        // maybe do this on demand rather than doing it for every texture and even unused ones?
        if(mAnimFrameCount > 1 && !isNextFrame())
        {
            mNextAnimationFrames.reserve(mAnimFrameCount-1);
            for(size_t i = 1; i < mAnimFrameCount; ++i)
            {
                AssetRef frameRef(getAssetId() + i, 0);
                auto frame = getDependencyTable()->loadAsset<Texture>(frameRef);
                mNextAnimationFrames.push_back(frame);
            }
        }

        // even if it's unlikely that this texture and it's material share an SRSC file, it would
        // not be hard to build such a file, and our library allows to create a TextureFactory and
        // a ClassFactory from the same SrscFile anyway. thus, we should load the material here instead
        // of loading it in load(...). better safe than sorry.
        if(!mMaterialClassRef.isNull())
        {
            mMaterialClass = getDependencyTable()->loadAsset<Class>(mMaterialClassRef);
        }
    }

    void Texture::_loadFromRecord(od::DataReader &dr)
    {
        Logger::debug() << "Loading texture " << std::hex << this->getAssetId() << std::dec;

        uint32_t rowSpacing;

        dr >> mWidth
           >> mHeight
           >> rowSpacing
           >> mBitsPerPixel
           >> mAlphaBitsPerPixel
           >> od::DataReader::Ignore(2)
           >> mColorKey
           >> mNextMipMapRef
           >> mAlternateRef
           >> mBumpMapRef
           >> mAnimationFps
           >> mFlags
           >> mMipMapNumber
           >> mMaterialClassRef
           >> mUsageCount
           >> mCompressionLevel
           >> mCompressedSize;

        mIsNextFrame = mFlags & OD_TEX_FLAG_NEXTFRAME;

        if(mFlags & OD_TEX_FLAG_ALPHAMAP)
        {
            Logger::error() << "Unsupported alpha map with " << mAlphaBitsPerPixel << "BPP";
            throw od::UnsupportedException("Alpha maps unsupported right now");
        }

        uint32_t trailingBytes = rowSpacing - mWidth*(mBitsPerPixel/8);
        if(trailingBytes)
        {
            throw od::UnsupportedException("Can only load packed textures right now");
        }

        if(mBitsPerPixel == 32)
        {
            Logger::warn() << "Found 32 bit texture. The used byte order of these varies between editor and engine. Colors may be garbled";

            if(mAlphaBitsPerPixel != 0 && mAlphaBitsPerPixel != 8)
            {
                Logger::warn() << "  That texture has an alpha channel bit depth that is not 8 bits. This is unimplemented. Gonna ignore alpha channel";
            }
        }

        // take apart color key so we can compare it more efficiently
        bool hasColorKey = (mColorKey != 0xffffffff);
        uint8_t keyRed   = (mColorKey & 0xff0000) >> 16;
        uint8_t keyGreen = (mColorKey & 0x00ff00) >> 8;
        uint8_t keyBlue  = (mColorKey & 0x0000ff);

        mHasAlphaChannel = (mAlphaBitsPerPixel != 0) || hasColorKey;

        std::unique_ptr<od::ZStream> zstr;
        od::DataReader zdr(dr);
        if(mCompressionLevel != 0)
        {
            // choose efficient output buffer sizes. ideally as much as we need exactly, but not more that what we'd use by default
            size_t uncompressedSize = mWidth*mHeight*(mBitsPerPixel/8) + mHeight*trailingBytes;
            size_t outputBufferSize = std::min(od::ZStreamBuffer::DefaultBufferSize, uncompressedSize);

            zstr = std::make_unique<od::ZStream>(dr.getStream(), mCompressedSize, outputBufferSize);
            zdr.setStream(*zstr);
        }

        std::function<void(unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)> pixelReaderFunc;
        if(mBitsPerPixel == 8)
        {
            pixelReaderFunc = [this, &zdr, hasColorKey, keyRed, keyGreen, keyBlue](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                uint8_t palIndex;
                zdr >> palIndex;

                TextureFactory::PaletteColor palColor = mTextureFactory.getPaletteColor(palIndex);

                red = palColor.red;
                green = palColor.green;
                blue = palColor.blue;

                if(hasColorKey && red == keyRed && green == keyGreen && blue == keyBlue)
                {
                    alpha = 0;

                }else
                {
                    alpha = OD_TEX_OPAQUE_ALPHA;
                }
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

            if(hasColorKey)
            {
                Logger::info() << "Found color key on 16 bpp texture. This is unsupported and will be ignored";
            }

            uint8_t rBits;
            uint8_t rShift;

            uint8_t gBits;
            uint8_t gShift;

            uint8_t bBits;
            uint8_t bShift;

            uint8_t aBits;
            uint8_t aShift;

            aBits = (mFlags & OD_TEX_FLAG_ALPHACHANNEL) ? mAlphaBitsPerPixel : 0;

            switch(aBits)
            {
            case 0:
                rBits = 5;
                gBits = 6;
                bBits = 5;
                break;

            case 1:
                rBits = 5;
                gBits = 5;
                bBits = 5;
                break;

            case 4:
                rBits = 4;
                gBits = 4;
                bBits = 4;
                break;

            case 8:
                rBits = 3;
                gBits = 3;
                bBits = 2;
                break;

            default:
                throw od::Exception("Invalid alpha BPP count");
            }

            aShift = rBits + gBits + bBits;
            rShift = gBits + bBits;
            gShift = bBits;
            bShift = 0;

            uint32_t rMask = (1 << rBits) - 1;
            uint32_t gMask = (1 << gBits) - 1;
            uint32_t bMask = (1 << bBits) - 1;
            uint32_t aMask = aBits != 0 ? (1 << aBits) - 1 : 0;

            pixelReaderFunc = [this, &zdr, rMask, rShift, gMask, gShift, bMask, bShift, aMask, aShift](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                uint16_t c;
                zdr >> c;

                red = _filter16BitChannel(c, rMask, rShift);
                green = _filter16BitChannel(c, gMask, gShift);
                blue = _filter16BitChannel(c, bMask, bShift);
                alpha = aMask ? _filter16BitChannel(c, aMask, aShift) : OD_TEX_OPAQUE_ALPHA;
            };

        }else if(mBitsPerPixel == 24)
        {
            pixelReaderFunc = [&zdr, hasColorKey, keyRed, keyGreen, keyBlue](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                zdr >> red
                    >> green
                    >> blue;

                if(hasColorKey && red == keyRed && green == keyGreen && blue == keyBlue)
                {
                    alpha = 0;

                }else
                {
                    alpha = OD_TEX_OPAQUE_ALPHA;
                }
            };

        }else if(mBitsPerPixel == 32)
        {
            pixelReaderFunc = [&zdr, this](unsigned char &red, unsigned char &green, unsigned char &blue, unsigned char &alpha)
            {
                // FIXME: the byte order created by the editor's convert function is RGBA, the one expected by the engine seems to be BGRA.
                //  since it is not entirely clear whether a level created for later versions of the Riot Engine would use RGBA or BGRA,
                //  we might need to change this order or make it depend on the SRSC version of the texture container.
                //  for now, stick with what seems to be expected by the engine.

                zdr >> blue
                    >> green
                    >> red
                    >> alpha;

                if(mAlphaBitsPerPixel != 8)
                {
                    alpha = OD_TEX_OPAQUE_ALPHA;
                }
            };

        }else
        {
            throw od::Exception("Invalid BPP");
        }

        // translate whatever is stored in texture into 8-bit RGBA format
        mRgba8888Data = std::make_unique<uint8_t[]>(mWidth*mHeight*4);
        for(size_t i = 0; i < mWidth*mHeight*4; i += 4)
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;

            pixelReaderFunc(red, green, blue, alpha);

            mRgba8888Data[i]   = red;
            mRgba8888Data[i+1] = green;
            mRgba8888Data[i+2] = blue;
            mRgba8888Data[i+3] = alpha;
        }

        if(zstr != nullptr)
        {
            zstr->seekToEndOfZlib();
        }

        Logger::debug() << "Texture successfully loaded";
    }

    unsigned char Texture::_filter16BitChannel(uint16_t color, uint32_t mask, uint32_t shift)
    {
        uint32_t channel = (color >> shift) & mask;

        return (channel*0xff)/mask;
    }

}

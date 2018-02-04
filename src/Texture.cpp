/*
 * Texture.cpp
 *
 *  Created on: 1 Feb 2018
 *      Author: zal
 */

#include "Texture.h"

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include "ZStream.h"
#include "Logger.h"
#include "Exception.h"
#include "TextureFactory.h"

namespace od
{

    Texture::Texture(RecordId id)
    : mId(id)
    , mWidth(0)
    , mHeight(0)
    , mBitsPerPixel(0)
    , mAlphaBitsPerPixel(0)
    {

    }

    void Texture::loadFromRecord(TextureFactory &factory, SrscFile &srscFile, SrscFile::DirIterator record)
    {
    	Logger::info() << "Loading texture " << mId;

        DataReader dr(srscFile.getStreamForRecord(record));

        uint32_t rowSpacing;

        dr >> mWidth
           >> mHeight
           >> rowSpacing
           >> mBitsPerPixel
           >> mAlphaBitsPerPixel;

        if(mAlphaBitsPerPixel)
        {
        	Logger::error() << "Unsupported alpha map with " << mAlphaBitsPerPixel << "BPP";
        	throw UnsupportedException("Alpha maps/channels unsupported right now");
        }

        dr.ignore(30);

        size_t bytePerPixel;
        switch(mBitsPerPixel)
        {
        case 8:
            bytePerPixel = 1;
            break;

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
            throw UnsupportedException("Can't handle uncompressed textures right now");
        }

        ZStream zstr(dr.getStream());
        DataReader zdr(zstr);

        // translate whatever is stored in texture into 24-bit format
        unsigned char *pixBuffer = new unsigned char[mWidth*mHeight*3]; // no need for RAII, osg takes ownership
        for(size_t i = 0; i < mWidth*mHeight*3; i += 3)
        {
        	// FIXME: one if per pixel is way too many!!!
        	if(bytePerPixel == 1)
        	{
        		uint8_t palIndex;
        		zdr >> palIndex;

        		TextureFactory::PaletteColor palColor = factory.getPaletteColor(palIndex);

        		pixBuffer[i]   = palColor.red;
        		pixBuffer[i+1] = palColor.green;
        		pixBuffer[i+2] = palColor.blue;

        	}else if(bytePerPixel == 2)
			{
        		uint16_t color;
        		zdr >> color;

        		pixBuffer[i]   = (color >> 8) & 0xf8;
        		pixBuffer[i+1] = (color >> 3) & 0xfc;
        		pixBuffer[i+2] = (color << 3) & 0xf8;

			}else if(bytePerPixel == 3)
			{
				zdr >> pixBuffer[i]
        		    >> pixBuffer[i+1]
        		    >> pixBuffer[i+2];

			}else if(bytePerPixel == 4)
			{
				// i know, this format makes little sense
				zdr >> pixBuffer[i+1]
        		    >> pixBuffer[i+2];

				zdr.ignore(1);

				zdr >> pixBuffer[i];
			}
        }
        zstr.seekToEndOfZlib();

        this->setImage(mWidth, mHeight, 1, 3, GL_RGB, GL_UNSIGNED_BYTE, pixBuffer, osg::Image::USE_NEW_DELETE);

        Logger::info() << "Texture successfully loaded";
    }

    void Texture::exportToPng(const FilePath &path)
    {
		osgDB::writeImageFile(*this, path.str());
    }

}

/*
 * TextureFactory.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "TextureFactory.h"

#include "Exception.h"
#include "SrscRecordTypes.h"
#include "Logger.h"

namespace od
{

	TextureFactory::TextureFactory(const FilePath &txdFilePath, Database &database)
	: mDatabase(database)
	, mSrscFile(txdFilePath)
	{
		_loadPalette();
	}

	TextureFactory::PaletteColor TextureFactory::getPaletteColor(size_t index)
	{
		if(index >= mPalette.size())
		{
			throw Exception("Color index not found in palette");
		}

		return mPalette[index];
	}

	TexturePtr TextureFactory::loadTexture(RecordId textureId)
	{
		SrscFile::DirIterator it = mSrscFile.getDirIteratorByTypeId(OD_SRSC_TEXTURE, textureId);
		if(it == mSrscFile.getDirectoryEnd())
		{
			throw NotFoundException("Texture not found in database");
		}

		// might as well just return a pointer to osg::Image, generate it right here and get rid of our Texture class
		TexturePtr texture(new Texture(textureId));
		texture->loadFromRecord(*this, mSrscFile, it);

		return texture;
	}

	void TextureFactory::_loadPalette()
	{
		Logger::verbose() << "Loading texture database palette";

		SrscFile::DirIterator it = mSrscFile.getDirIteratorByType(OD_SRSC_PALETTE);
		if(it == mSrscFile.getDirectoryEnd())
		{
			return;
		}

		DataReader dr(mSrscFile.getStreamForRecord(it));

		uint16_t colorCount;
		dr >> colorCount;

		mPalette.reserve(colorCount);
		for(size_t i = 0; i < colorCount; ++i)
		{
			PaletteColor color;

			dr >> color.red
			   >> color.green
			   >> color.blue;

			mPalette.push_back(color);
		}
	}
}

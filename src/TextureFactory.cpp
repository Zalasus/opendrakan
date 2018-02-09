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
#include "Database.h"

namespace od
{

	TextureFactory::TextureFactory(const FilePath &txdFilePath, Database &database)
	: AssetFactory<Texture>(txdFilePath, database)
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

	TexturePtr TextureFactory::loadAsset(RecordId textureId)
	{
		SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(OD_SRSC_TEXTURE, textureId);
		if(dirIt == getSrscFile().getDirectoryEnd())
		{
			throw NotFoundException("Texture not found in database");
		}

		Logger::verbose() << "Loading texture " << std::hex << textureId << std::dec << " from database '" << getDatabase().getDbFilePath().fileStrNoExt() << "'";

		TexturePtr texture(new Texture(getDatabase(), textureId));
		texture->loadFromRecord(*this, DataReader(getSrscFile().getStreamForRecord(dirIt)));

		return texture;
	}

	void TextureFactory::_loadPalette()
	{
		Logger::verbose() << "Loading texture database palette";

		SrscFile::DirIterator it = getSrscFile().getDirIteratorByType(OD_SRSC_PALETTE);
		if(it == getSrscFile().getDirectoryEnd())
		{
			return;
		}

		DataReader dr(getSrscFile().getStreamForRecord(it));

		uint16_t colorCount;
		dr >> colorCount;

		mPalette.reserve(colorCount);
		for(size_t i = 0; i < colorCount; ++i)
		{
			PaletteColor color;

			dr >> color.red
			   >> color.green
			   >> color.blue
			   >> color.dummy;

			mPalette.push_back(color);
		}
	}
}

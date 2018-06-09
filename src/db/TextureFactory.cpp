/*
 * TextureFactory.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "db/TextureFactory.h"

#include "Exception.h"
#include "SrscRecordTypes.h"
#include "Logger.h"

namespace od
{

	TextureFactory::TextureFactory(AssetProvider &ap, SrscFile &textureContainer, Engine &engine)
	: AssetFactory<Texture>(ap, textureContainer)
	, mEngine(engine)
	{
		_loadPalette();
	}

	TextureFactory::PaletteColor TextureFactory::getPaletteColor(size_t index)
	{
		if(index >= mPalette.size())
		{
			PaletteColor black = { .red = 0, .green = 0, .blue = 0, .dummy = 0 };
			return black;
		}

		return mPalette[index];
	}

	osg::ref_ptr<Texture> TextureFactory::loadAsset(RecordId textureId)
	{
		SrscFile::DirIterator dirIt = getSrscFile().getDirIteratorByTypeId(SrscRecordType::TEXTURE, textureId);
		if(dirIt == getSrscFile().getDirectoryEnd())
		{
			return nullptr;
		}

		osg::ref_ptr<Texture> texture(new Texture(getAssetProvider(), textureId));
		texture->loadFromRecord(*this, DataReader(getSrscFile().getStreamForRecord(dirIt)));

		return texture;
	}

	void TextureFactory::_loadPalette()
	{
		Logger::verbose() << "Loading texture database palette";

		SrscFile::DirIterator it = getSrscFile().getDirIteratorByType(SrscRecordType::PALETTE);
		if(it == getSrscFile().getDirectoryEnd())
		{
			Logger::warn() << "Texture container has no palette record. 8 bit textures will be blank";
			return;
		}

		DataReader dr(getSrscFile().getStreamForRecord(it));

		uint16_t colorCount;
		dr >> colorCount;

		if(!colorCount)
		{
			Logger::warn() << "Texture container palette contains no colors. 8 bit textures will be blank";
		}

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

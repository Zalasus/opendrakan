/*
 * TextureFactory.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include <odCore/db/TextureFactory.h>

#include <odCore/SrscRecordTypes.h>
#include <odCore/Logger.h>

namespace odDb
{

    static TextureFactory::PaletteColor UNSPECIFIED_PALETTE_COLOR = { 0, 0, 0, 0 };


	TextureFactory::TextureFactory(std::shared_ptr<DependencyTable> depTable, od::SrscFile &textureContainer)
	: AssetFactory<Texture>(depTable, textureContainer)
	{
		_loadPalette();
	}

	TextureFactory::PaletteColor TextureFactory::getPaletteColor(size_t index)
	{
		if(index >= mPalette.size())
		{
			return UNSPECIFIED_PALETTE_COLOR;
		}

		return mPalette[index];
	}

	std::shared_ptr<Texture> TextureFactory::createNewAsset(od::RecordId id)
	{
		return std::make_shared<Texture>(*this);
	}

	void TextureFactory::_loadPalette()
	{
		Logger::verbose() << "Loading texture database palette";

		auto cursor = getSrscFile().getFirstRecordOfType(od::SrscRecordType::PALETTE);
		if(!cursor.isValid())
		{
			Logger::warn() << "Texture container has no palette record. 8 bit textures will be blank";
			return;
		}

		od::DataReader dr = cursor.getReader();

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

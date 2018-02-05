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

	TextureFactory::~TextureFactory()
	{
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
	    auto it = mTextureCache.find(textureId);
	    if(it != mTextureCache.end())
	    {
	        Logger::debug() << "Texture " << std::hex << textureId << std::dec << " found in cache";

	        return TexturePtr(it->second);
	    }

		SrscFile::DirIterator dirIt = mSrscFile.getDirIteratorByTypeId(OD_SRSC_TEXTURE, textureId);
		if(dirIt == mSrscFile.getDirectoryEnd())
		{
			throw NotFoundException("Texture not found in database");
		}

		TexturePtr texture(new Texture(textureId));
		texture->loadFromRecord(*this, mSrscFile, dirIt);
		texture->addObserver(this);

		mTextureCache[textureId] = texture.get();
		Logger::debug() << "Texture factory now caching texture with ID " << std::hex << textureId << std::dec;

		return texture;
	}

	void TextureFactory::objectDeleted(void *object)
	{
	    Texture *texture = dynamic_cast<Texture*>(static_cast<osg::Referenced*>(object)); // a bit unsafe but osg gives us no choice
	    if(texture == nullptr)
	    {
	        Logger::warn() << "Texture factory was notified of deletion of non-texture object";
	        return;
	    }

	    RecordId texId = texture->getRecordId();

	    Logger::debug() << "Unregistering texture " << std::hex << texId << std::dec << " from texture cache";

	    mTextureCache.erase(texId);
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

/*
 * TextureFactory.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_TEXTUREFACTORY_H_
#define INCLUDE_TEXTUREFACTORY_H_

#include <osg/Observer>

#include "FilePath.h"
#include "SrscFile.h"
#include "Texture.h"

namespace od
{
	class Database;

	class TextureFactory : public osg::Observer
	{
	public:

		struct PaletteColor
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t dummy;
		};

		TextureFactory(const FilePath &txdFilePath, Database &database);
		virtual ~TextureFactory();

		PaletteColor getPaletteColor(size_t index);

		TexturePtr loadTexture(RecordId textureId);


		// override osg::Observer
		virtual void objectDeleted(void *object);


	private:

		void _loadPalette();

		Database &mDatabase;
		SrscFile mSrscFile;

		std::vector<PaletteColor> mPalette;
        std::map<RecordId, Texture*> mTextureCache;
	};

}

#endif /* INCLUDE_TEXTUREFACTORY_H_ */

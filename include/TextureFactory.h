/*
 * TextureFactory.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_TEXTUREFACTORY_H_
#define INCLUDE_TEXTUREFACTORY_H_

#include "FilePath.h"
#include "SrscFile.h"
#include "Texture.h"

namespace od
{
	class Database;

	class TextureFactory
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

		PaletteColor getPaletteColor(size_t index);

		TexturePtr loadTexture(RecordId textureId);


	private:

		void _loadPalette();

		Database &mDatabase;
		SrscFile mSrscFile;

		std::vector<PaletteColor> mPalette;

	};

}

#endif /* INCLUDE_TEXTUREFACTORY_H_ */

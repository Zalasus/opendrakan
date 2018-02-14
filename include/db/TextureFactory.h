/*
 * TextureFactory.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_TEXTUREFACTORY_H_
#define INCLUDE_TEXTUREFACTORY_H_

#include <osg/Observer>

#include "Asset.h"
#include "AssetFactory.h"
#include "FilePath.h"
#include "SrscFile.h"
#include "Texture.h"

namespace od
{
	class Database;

	class TextureFactory : public AssetFactory<Texture>
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


	protected:

		// implement AssetFactory<Texture>
		TexturePtr loadAsset(RecordId textureId) override;


	private:

		void _loadPalette();

		std::vector<PaletteColor> mPalette;
	};

}

#endif /* INCLUDE_TEXTUREFACTORY_H_ */

/*
 * AssetProvider.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSETPROVIDER_H_
#define INCLUDE_ASSETPROVIDER_H_

#include "Asset.h"
#include "Texture.h"
#include "Class.h"
#include "Model.h"
#include "Exception.h"

namespace od
{

	/**
	 * Common interface for classes with means to translate DB indices to
	 * actual databases, like Database and Level.
	 */
	class AssetProvider
	{
	public:

		virtual ~AssetProvider() {};

	    /*
	     * These get assets from this provider or fetch them from a referenced dependency.
	     */
	    virtual TexturePtr getTextureByRef(const AssetRef &ref) = 0;
	    virtual ClassPtr   getClassByRef(const AssetRef &ref) = 0;
	    virtual ModelPtr   getModelByRef(const AssetRef &ref) = 0;

	    /*
	     * These get assets from this provider without indirection. Override if implementer can load
	     * given asset from itself.
	     */
	    virtual TexturePtr getTexture(RecordId recordId) { throw Exception("Can't provide textures"); }
	    virtual ClassPtr   getClass(RecordId recordId) { throw Exception("Can't provide textures"); }
	    virtual ModelPtr   getModel(RecordId recordId) { throw Exception("Can't provide textures"); }

	};


}

#endif /* INCLUDE_ASSETPROVIDER_H_ */

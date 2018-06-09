/*
 * AssetProvider.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSETPROVIDER_H_
#define INCLUDE_ASSETPROVIDER_H_

#include "Asset.h"
#include "Exception.h"

namespace od
{

    class Texture;
    class Class;
    class Model;
    class Sequence;
    class Animation;
    class Sound;

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
	     * Note that these functions transfer ownership. It is vital that their return value is stored in a ref_ptr.
	     */
	    virtual Texture   *getTextureByRef(const AssetRef &ref) { throw Exception("Can't provide textures"); }
	    virtual Class     *getClassByRef(const AssetRef &ref) { throw Exception("Can't provide classes"); }
	    virtual Model     *getModelByRef(const AssetRef &ref) { throw Exception("Can't provide models"); }
	    virtual Sequence  *getSequenceByRef(const AssetRef &ref) { throw Exception("Can't provide sequences"); }
	    virtual Animation *getAnimationByRef(const AssetRef &ref) { throw Exception("Can't provide animations"); }
	    virtual Sound     *getSoundByRef(const AssetRef &ref) { throw Exception("Can't provide sounds"); }

	    /*
	     * These get assets from this provider without indirection. Override if implementer can load
	     * given asset from itself (basically only databases).
	     * Note that these functions transfer ownership. It is vital that their return value is stored in a ref_ptr.
	     */
	    virtual Texture   *getTexture(RecordId recordId) { throw Exception("Can't provide textures"); }
	    virtual Class     *getClass(RecordId recordId) { throw Exception("Can't provide classes"); }
	    virtual Model     *getModel(RecordId recordId) { throw Exception("Can't provide models"); }
	    virtual Sequence  *getSequence(RecordId recordId) { throw Exception("Can't provide sequences"); }
	    virtual Animation *getAnimation(RecordId recordId) { throw Exception("Can't provide animations"); }
	    virtual Sound     *getSound(RecordId recordId) { throw Exception("Can't provide sounds"); }

	    template <typename _AssetType>
	    _AssetType *getAssetByRef(const AssetRef &ref);

	};

	template<>
    Texture *AssetProvider::getAssetByRef<Texture>(const AssetRef &ref);

	template<>
    Class *AssetProvider::getAssetByRef<Class>(const AssetRef &ref);

	template<>
    Model *AssetProvider::getAssetByRef<Model>(const AssetRef &ref);

	template<>
    Sequence *AssetProvider::getAssetByRef<Sequence>(const AssetRef &ref);

	template<>
    Animation *AssetProvider::getAssetByRef<Animation>(const AssetRef &ref);

	template<>
    Sound *AssetProvider::getAssetByRef<Sound>(const AssetRef &ref);


}

#endif /* INCLUDE_ASSETPROVIDER_H_ */

/*
 * Asset.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_ASSET_H_
#define INCLUDE_ASSET_H_

#include <odCore/SrscFile.h>

namespace odDb
{

	class AssetProvider;
	class AssetReferenceObserver;

	class Asset
	{
	public:

		Asset(AssetProvider &ap, od::RecordId assetId);
		Asset(Asset &a) = delete;
		Asset(const Asset &a) = delete;
		virtual ~Asset();

		void referenceCreated();
		void referenceDestroyed();
		void referenceReleased();
		void setReferenceObserver(AssetReferenceObserver *observer);

		inline od::RecordId getAssetId() const { return mId; }
		inline AssetProvider &getAssetProvider() { return mAssetProvider; };


	private:

		AssetProvider &mAssetProvider;
		od::RecordId mId;

	};

	template <typename T>
	struct AssetTraits
	{
	    static const char *name();
	};

	/**
	 * Structure for representing a Record-ID & Database-Index pair as they
	 * appear all over the engines files when referencing another asset.
	 */
	struct AssetRef
	{
		AssetRef() : assetId(0), dbIndex(0) {}
		AssetRef(od::RecordId id, uint16_t index) : assetId(id), dbIndex(index) {}

		od::RecordId assetId;
		uint16_t dbIndex;

		// so we can use this in a map
		bool operator<(const AssetRef &right) const;

		bool operator==(const AssetRef &right) const;
		inline bool operator!=(const AssetRef &right) const { return !(this->operator==(right)); }

		/**
		 * @brief Checks if this is a null reference i.e. not referencing anything. Not applicable to layer textures.
		 */
		inline bool isNull() const { return dbIndex == 0 && assetId == 0; }

		static const AssetRef NULL_REF;
	};

	od::DataReader &operator>>(od::DataReader &left, AssetRef &right);

	std::ostream &operator<<(std::ostream &left, const AssetRef &right);


	class AssetReferenceObserver
    {
    protected:

        friend class Asset;

        virtual ~AssetReferenceObserver() = default;

        virtual void onLastReferenceDestroyed(Asset *asset) = 0;

    };


	template <typename _AssetType>
    class AssetPtr
    {
    public:

        static_assert(std::is_base_of<Asset, _AssetType>::value, "_AssetType in odDb::AssetPtr<_AssetType> must have odDb::Asset as a base!");

        AssetPtr()
        : mPtr(nullptr)
        {
        }

        AssetPtr(_AssetType *ptr)
        : mPtr(ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }
        }

        AssetPtr(const AssetPtr<_AssetType> &refPtr)
        : mPtr(refPtr.mPtr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }
        }

        typename AssetPtr<_AssetType> &operator=(_AssetType *ptr)
        {
            if(mPtr != nullptr)
            {
                mPtr->referenceDestroyed();
            }

            mPtr = ptr;

            if(mPtr != nullptr)
            {
                mPtr->referenceCreated();
            }

            return *this;
        }

        typename AssetPtr<_AssetType> &operator=(const AssetPtr<_AssetType> &refPtr)
        {
            return this->operator=(refPtr.mPtr);
        }

        _AssetType *operator _AssetType*() const
        {
            return mPtr;
        }

        _AssetType *operator->() const
        {
            return mPtr;
        }

        _AssetType *release()
        {
            _AssetType *ptr = mPtr;
            mPtr = nullptr;

            if(ptr != nullptr)
            {
                mPtr->referenceReleased();
            }

            return ptr;
        }

        _AssetType *get()
        {
            return mPtr;
        }


    private:

        _AssetType *mPtr;

    };

}

#endif /* INCLUDE_ASSET_H_ */



#ifndef INCLUDE_DB_ASSETREF_H_
#define INCLUDE_DB_ASSETREF_H_

#include <odCore/SrscFile.h>

#include <odCore/db/IdTypes.h>

namespace odDb
{

    /**
	 * Structure for representing a Record-ID & Database-Index pair as they
	 * appear all over the engines files when referencing another asset.
     *
     * The database index in these is realtive to the dependency table of the
     * database they came from.
	 */
	struct AssetRef
	{
		AssetRef()
        : assetId(0)
        , dbIndex(0)
        {
        }

		AssetRef(od::RecordId id, DatabaseIndex index)
        : assetId(id)
        , dbIndex(index)
        {
        }

		od::RecordId assetId;
		DatabaseIndex dbIndex;

		bool operator==(const AssetRef &right) const;
		inline bool operator!=(const AssetRef &right) const { return !(this->operator==(right)); }

		/**
		 * @brief Checks if this is a null reference i.e. not referencing anything. Not applicable to layer textures.
		 */
		inline bool isNull() const { return dbIndex == 0 && assetId == 0; }

		static constexpr od::RecordId NULL_ASSET{ 0 };
        static constexpr DatabaseIndex SELF_DBINDEX{0};
        static const AssetRef NULL_REF;
	};


    struct AssetRefHasher
    {
        size_t operator()(const AssetRef &ref) const;
    };


	std::ostream &operator<<(std::ostream &left, const AssetRef &right);


    /**
     * @brief An alternative asset ref which references a database using it's global index instead
     */
    struct GlobalAssetRef
	{
        od::RecordId assetId;
		GlobalDatabaseIndex globalDbIndex;
    };

}

#endif

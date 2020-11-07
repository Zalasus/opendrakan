
#include <odCore/db/AssetRef.h>

#include <functional>

namespace odDb
{

    bool AssetRef::operator==(const AssetRef &right) const
	{
    	return (dbIndex == right.dbIndex) && (assetId == right.assetId);
	}

    const AssetRef AssetRef::NULL_REF{ AssetRef::NULL_ASSET, AssetRef::SELF_DBINDEX };

    size_t AssetRefHasher::operator()(const AssetRef &ref) const
    {
        std::hash<uint32_t> hash;
        uint32_t intRepresentation = (ref.assetId << 16) | ref.dbIndex;
        return hash(intRepresentation);
    }


    std::ostream &operator<<(std::ostream &left, const AssetRef &right)
    {
        left << std::hex << right.dbIndex << ":" << right.assetId << std::dec;

        return left;
    }

}

namespace od
{

    template <>
    void DataReader::readTyped<odDb::AssetRef>(odDb::AssetRef &ref)
    {
        (*this) >> ref.assetId >> ref.dbIndex;
    }

    template <>
    void DataWriter::writeTyped<odDb::AssetRef>(const odDb::AssetRef &ref)
    {
        (*this) << ref.assetId << ref.dbIndex;
    }


    template <>
    void DataReader::readTyped<odDb::GlobalAssetRef>(odDb::GlobalAssetRef &ref)
    {
        (*this) >> ref.assetId >> ref.globalDbIndex;
    }

    template <>
    void DataWriter::writeTyped<odDb::GlobalAssetRef>(const odDb::GlobalAssetRef &ref)
    {
        (*this) << ref.assetId << ref.globalDbIndex;
    }

}

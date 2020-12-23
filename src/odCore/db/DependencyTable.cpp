
#include <odCore/db/DependencyTable.h>

#include <odCore/db/Database.h>

namespace odDb
{

    DependencyTable::DependencyTable()
    : mSelfRefDatabase(nullptr)
    {
    }

    DependencyTable::DependencyTable(std::weak_ptr<Database> selfRefDb)
    : mSelfRefDatabase(selfRefDb)
    {
    }

    std::optional<GlobalAssetRef> DependencyTable::localToGlobalRef(const AssetRef &ref) const
    {
        auto db = getDependency(ref.dbIndex);
        if(db == nullptr)
        {
            return {};

        }else
        {
            return GlobalAssetRef(ref.assetId, db->getGlobalIndex());
        }
    }

    std::shared_ptr<Database> DependencyTable::getDependency(DatabaseIndex index) const
    {
        if(index == AssetRef::SELF_DBINDEX)
        {
            return mSelfRefDatabase.lock();

        }else
        {
            auto it = mDependencyMap.find(index);
            if(it == mDependencyMap.end())
            {
                return nullptr;

            }else
            {
                return it->second;
            }
        }
    }

    void DependencyTable::reserveDependencies(size_t n)
    {
        mDependencyMap.reserve(n);
    }

    void DependencyTable::addDependency(DatabaseIndex index, std::shared_ptr<Database> db)
    {
        mDependencyMap.insert(std::make_pair(index, db));
    }

}

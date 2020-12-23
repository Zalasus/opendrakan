
#ifndef INCLUDE_ODCORE_DB_DEPENDENCYTABLE_H_
#define INCLUDE_ODCORE_DB_DEPENDENCYTABLE_H_

#include <unordered_map>

#include <odCore/db/AssetRef.h>
#include <odCore/db/Database.h>

namespace odDb
{
    
    class DependencyTable
    {
    public:

        DependencyTable();
        DependencyTable(std::weak_ptr<Database> selfRefDb);

        inline size_t getDependencyCount() const { return mDependencyMap.size(); }

        std::optional<GlobalAssetRef> localToGlobalRef(const AssetRef &ref) const;
        std::shared_ptr<Database> getDependency(DatabaseIndex index) const;

        template <typename T>
        std::shared_ptr<T> loadAsset(const AssetRef &ref) const
        {
            auto db = getDependency(ref.dbIndex);
            if(db != nullptr)
            {
                return db->loadAsset<T>(ref.assetId);

            }else
            {
                Logger::warn() << "Invalid depdendency index in asset reference: " << ref.dbIndex;
                return nullptr;
            }
        }

        void reserveDependencies(size_t n);
        void addDependency(DatabaseIndex index, std::shared_ptr<Database> db);


    private:

        std::weak_ptr<Database> mSelfRefDatabase;
        std::unordered_map<DatabaseIndex, std::shared_ptr<Database>> mDependencyMap;

    };

}

#endif

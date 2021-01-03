
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

        /**
         * @brief Assigns the Database which is referred to by index 0.
         *
         * TODO: i dislike this way of handling the 0-refs very much. but we need *some* way of giving users of assets a way
         *  of loading referenced assets without knowing the database. and also not every asset comes from a database. and not every
         *  asset ref references a database dependency table.
         */
        void setSelfRefDatabase(std::weak_ptr<Database> db);


    private:

        std::weak_ptr<Database> mSelfRefDatabase;
        std::unordered_map<DatabaseIndex, std::shared_ptr<Database>> mDependencyMap;

    };

}

#endif

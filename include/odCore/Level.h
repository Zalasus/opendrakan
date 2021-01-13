/*
 * RiotLevel.h
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#ifndef RIOTLEVEL_H_
#define RIOTLEVEL_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <glm/vec3.hpp>

#include <odCore/IdTypes.h>
#include <odCore/Engine.h>
#include <odCore/FilePath.h>
#include <odCore/ObjectRecord.h>

#include <odCore/rfl/Class.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>
#include <odCore/db/DependencyTable.h>

namespace od
{
    class LevelObject;
    class Layer;

    class Level
    {
    public:

        Level(Engine engine);
        ~Level();

        inline Engine &getEngine() { return mEngine; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return mPhysicsSystem; }
        inline float getVerticalExtent() const { return mVerticalExtent; } ///< @return The distance between the lowest and the highest point in terrain
        inline std::shared_ptr<odDb::DependencyTable> getDependencyTable() const { return mDependencyTable; }

        /**
         * @brief Loads a level from the given file.
         * @param levelPath  A path to the .lvl file.
         * @param dbManager  A DbManager from which to load databases the level depends on.
         */
        void loadLevel(const FilePath &levelPath, odDb::DbManager &dbManager);

        void addToDestructionQueue(LevelObjectId objId);

        Layer *getLayerById(LayerId id);
        Layer *getLayerByIndex(uint16_t index); // FIXME: this should be removed (if possible). the index is really only correct during loading
        void findAdjacentAndOverlappingLayers(Layer *checkLayer, std::vector<Layer*> &results);

        /**
         * @brief Spawns all objects with SpawnStrategy::Always.
         */
        void initialSpawn();

        /**
         * @brief Spawns all objects, regardless of their visibility and SpawnStrategy.
         */
        void spawnAllObjects();

        void update(float relTime);

        /**
         * @brief Returns the record data for a given object record index (as encountered during loading).
         *
         * Will throw if the index is out of bounds.
         */
        ObjectRecordData &getObjectRecord(uint16_t index);

        /**
         * @brief Returns the object ID for a given object record index (as encountered during loading).
         *
         * Will throw if the index is out of bounds.
         */
        inline LevelObjectId getObjectIdForRecordIndex(uint16_t index) { return getObjectRecord(index).getObjectId(); }


        std::shared_ptr<LevelObject> getLevelObjectById(LevelObjectId id);

        /**
         * @brief Finds the first object with the given class type.
         *
         * @return The first object of class type \c id or nullptr if none found.
         */
        std::shared_ptr<LevelObject> findFirstObjectOfType(odRfl::ClassId id);

        /**
         * @brief Finds all objects with the given class type and adds them to the provided vector.
         */
        void findObjectsOfType(odRfl::ClassId id, std::vector<std::shared_ptr<LevelObject>> &results);

        void activateLayerPVS(Layer *layer);

        void calculateInitialLayerAssociations();

        template <typename F>
        void forEachObject(const F &f)
        {
            for(auto &objectIt : mLevelObjects)
            {
                f(*(objectIt.second));
            }
        }


    private:

        void _loadNameAndDeps(SrscFile &file, odDb::DbManager &dbManager);
        void _loadLayers(SrscFile &file);
        void _loadLayerGroups(SrscFile &file);
        void _loadObjects(SrscFile &file, odDb::DbManager &dbManage);

        Engine mEngine;
        odPhysics::PhysicsSystem &mPhysicsSystem;
        odRender::Renderer *mRenderer;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::shared_ptr<odDb::DependencyTable> mDependencyTable;

        // TODO: we could potentially share this across local clients/servers. would go well with separation of data and state
        std::vector<ObjectRecordData> mObjectRecords;

		float mVerticalExtent;
		Layer *mCurrentActivePvsLayer;

        std::unordered_set<LevelObjectId> mDestructionQueue;

        std::vector<std::unique_ptr<Layer>> mLayers;
        std::unordered_map<LevelObjectId, std::shared_ptr<LevelObject>> mLevelObjects;
    };


}

#endif /* RIOTLEVEL_H_ */

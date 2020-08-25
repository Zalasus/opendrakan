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

#include <odCore/rfl/Class.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

namespace od
{
    class LevelObject;
    class Layer;

    class Level : public odDb::AssetProvider
    {
    public:

        /**
         * @brief Loads a level from the given file.
         * @param levelPath  A path to the .lvl file.
         * @param dbManager  A DbManager from which to load databases the level depends on.
         */
        Level(Engine engine);
        ~Level();

        inline Engine &getEngine() { return mEngine; }
        inline odPhysics::PhysicsSystem &getPhysicsSystem() { return mPhysicsSystem; }
        inline float getVerticalExtent() const { return mVerticalExtent; } ///< @return The distance between the lowest and the highest point in terrain


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

        LevelObject *getLevelObjectById(LevelObjectId id);

        /**
         * @brief Finds the first object with the given class type.
         * TODO: change LevelObject storage to shared_ptr and replace these pointers
         * @return The first object of class type \c id or nullptr if none found.
         */
        LevelObject *findFirstObjectOfType(odRfl::ClassId id);

        /**
         * @brief Finds all objects with the given class type and adds them to the provided vector.
         * TODO: change LevelObject storage to shared_ptr and replace these pointers
         */
        void findObjectsOfType(odRfl::ClassId id, std::vector<LevelObject*> &results);

        // override AssetProvider
        virtual AssetProvider &getDependency(uint16_t index) override;

        void activateLayerPVS(Layer *layer);

        void calculateInitialLayerAssociations();

        void addToUpdateQueue(LevelObject &obj);
        void removeFromUpdateQueue(LevelObject &obj);


    private:

        void _loadNameAndDeps(SrscFile &file, odDb::DbManager &dbManager);
        void _loadLayers(SrscFile &file);
        void _loadLayerGroups(SrscFile &file);
        void _loadObjects(SrscFile &file);

        Engine mEngine;
        odPhysics::PhysicsSystem &mPhysicsSystem;
        odRender::Renderer *mRenderer;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::map<uint16_t, std::shared_ptr<odDb::Database>> mDependencyMap;

		float mVerticalExtent;
		Layer *mCurrentActivePvsLayer;

        std::unordered_set<LevelObjectId> mDestructionQueue;

		// since an object could add/remove itself from the update queue during the update phase,
        // we have to consider that any iterators of mObjectUpdateQueue might become invalid while
		// performing the update. thus, we copy the queue into a second, temporary vector during update
		// so no invalidation can occur.
        std::unordered_set<LevelObjectId> mObjectUpdateQueue;
        std::unordered_set<LevelObjectId> mTempObjectUpdateQueue;

        std::vector<std::unique_ptr<Layer>> mLayers;
        std::unordered_map<LevelObjectId, std::unique_ptr<LevelObject>> mLevelObjects;
    };


}

#endif /* RIOTLEVEL_H_ */

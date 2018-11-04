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
#include <list>
#include <glm/vec3.hpp>

#include <odCore/FilePath.h>
#include <odCore/Layer.h>
#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>
#include <odCore/physics/PhysicsManager.h>

namespace od
{

	class Engine;

    class Level : public odDb::AssetProvider
    {
    public:

        Level(const FilePath &levelPath, Engine &engine);
        ~Level();

        inline FilePath getFilePath() const { return mLevelPath; }
        inline Engine &getEngine() { return mEngine; }
        inline odPhysics::PhysicsManager &getPhysicsManager() { return mPhysicsManager; }

        void loadLevel();
        void spawnAllObjects();
        void requestLevelObjectDestruction(LevelObject *obj);
        Layer *getLayerById(uint32_t id);
        Layer *getLayerByIndex(uint16_t index);
        Layer *getFirstLayerBelowPoint(const glm::vec3 &v);
        void findAdjacentAndOverlappingLayers(Layer *checkLayer, std::vector<Layer*> &results);

        void update();

        LevelObject *getLevelObjectByIndex(uint16_t index);

        // override AssetProvider
        virtual AssetProvider &getDependency(uint16_t index) override;


    private:

        void _loadNameAndDeps(SrscFile &file);
        void _loadLayers(SrscFile &file);
        void _loadLayerGroups(SrscFile &file);
        void _loadObjects(SrscFile &file);


        FilePath mLevelPath;
        Engine &mEngine;
        odDb::DbManager &mDbManager;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::map<uint16_t, odDb::DbRefWrapper> mDependencyMap;
        std::vector<std::unique_ptr<Layer>> mLayers;
        std::vector<std::unique_ptr<LevelObject>> mLevelObjects;
		odPhysics::PhysicsManager mPhysicsManager;

		std::list<LevelObject*> mDestructionQueue;

		std::vector<Layer*> mLayerLookupCache;
    };


}

#endif /* RIOTLEVEL_H_ */

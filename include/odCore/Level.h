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

#include <odCore/rfl/RflClass.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

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
        inline float getVerticalExtent() const { return mVerticalExtent; } ///< @return The distance between the lowest and the highest point in terrain

        void loadLevel();
        void spawnAllObjects();
        void requestLevelObjectDestruction(LevelObject *obj);
        Layer *getLayerById(uint32_t id);
        Layer *getLayerByIndex(uint16_t index);
        void findAdjacentAndOverlappingLayers(Layer *checkLayer, std::vector<Layer*> &results);

        void update(float relTime);

        LevelObject *getLevelObjectByIndex(uint16_t index);

        /**
         * @brief Finds the first object with the given class type.
         *
         * @return The first object of class type \c id or nullptr if none found.
         */
        LevelObject *findObjectOfType(odRfl::RflClassId id);

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

		std::list<LevelObject*> mDestructionQueue;

		float mVerticalExtent;
    };


}

#endif /* RIOTLEVEL_H_ */

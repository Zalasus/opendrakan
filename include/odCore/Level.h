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
#include <deque>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Light>

#include <odCore/FilePath.h>
#include <odCore/Layer.h>
#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>
#include <odCore/physics/PhysicsManager.h>

namespace od
{

	class Engine;

    class Level : public AssetProvider
    {
    public:

        Level(const FilePath &levelPath, Engine &engine, osg::ref_ptr<osg::Group> levelRootNode);
        ~Level();

        inline FilePath getFilePath() const { return mLevelPath; }
        inline Engine &getEngine() { return mEngine; }
        inline PhysicsManager &getPhysicsManager() { return mPhysicsManager; }

        void loadLevel();
        void spawnAllObjects();
        void requestLevelObjectDestruction(LevelObject *obj);
        Layer *getLayerById(uint32_t id);
        Layer *getLayerByIndex(uint16_t index);
        Layer *getFirstLayerBelowPoint(const osg::Vec3 &v);
        void findAdjacentAndOverlappingLayers(Layer *checkLayer, std::vector<Layer*> &results);

        void update();

        LevelObject &getLevelObjectByIndex(uint16_t index);

        // override AssetProvider
        virtual AssetProvider &getDependency(uint16_t index) override;


    private:

        void _loadNameAndDeps(SrscFile &file);
        void _loadLayers(SrscFile &file);
        void _loadLayerGroups(SrscFile &file);
        void _loadObjects(SrscFile &file);


        FilePath mLevelPath;
        Engine &mEngine;
        DbManager &mDbManager;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::map<uint16_t, DbRefWrapper> mDependencyMap;
        std::vector<osg::ref_ptr<Layer>> mLayers;
        std::vector<osg::ref_ptr<LevelObject>> mLevelObjects;
        osg::ref_ptr<osg::Group> mLevelRootNode;
        osg::ref_ptr<osg::Group> mLayerGroup;
        osg::ref_ptr<osg::Group> mObjectGroup;
        osg::ref_ptr<osg::Light> mSunLight;
		PhysicsManager mPhysicsManager;

		std::deque<osg::ref_ptr<LevelObject>> mDestructionQueue;

		std::vector<Layer*> mLayerLookupCache;
    };


}

#endif /* RIOTLEVEL_H_ */

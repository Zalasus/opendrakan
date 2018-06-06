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

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Light>

#include "FilePath.h"
#include "DbManager.h"
#include "db/Database.h"
#include "TextureAtlas.h"
#include "Layer.h"
#include "Player.h"
#include "physics/PhysicsManager.h"

namespace od
{

	class Engine;

    class Level : public AssetProvider
    {
    public:

        Level(const FilePath &levelPath, Engine &engine, osg::ref_ptr<osg::Group> levelRootNode);
        ~Level();

        inline Player *getPlayer() { return mPlayer; }
        inline void setPlayer(Player *p) { mPlayer = p; }
        inline FilePath getFilePath() const { return mLevelPath; }
        inline Engine &getEngine() { return mEngine; }
        inline PhysicsManager &getPhysicsManager() { return mPhysicsManager; }

        void loadLevel();
        void spawnAllObjects();

        // implement AssetProvider
        virtual Texture   *getTextureByRef(const AssetRef &ref) override;
        virtual Class     *getClassByRef(const AssetRef &ref) override;
        virtual Model     *getModelByRef(const AssetRef &ref) override;
        virtual Sequence  *getSequenceByRef(const AssetRef &ref) override;
        virtual Animation *getAnimationByRef(const AssetRef &ref) override;
        virtual Sound     *getSoundByRef(const AssetRef &ref) override;


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
        std::vector<std::shared_ptr<Layer>> mLayers;
        std::map<uint32_t, osg::ref_ptr<LevelObject>> mLevelObjects;
        osg::ref_ptr<osg::Group> mLevelRootNode;
        osg::ref_ptr<osg::Group> mLayerGroup;
        osg::ref_ptr<osg::Group> mObjectGroup;
        osg::ref_ptr<osg::Light> mSunLight;
		Player *mPlayer;
		PhysicsManager mPhysicsManager;
    };


}

#endif /* RIOTLEVEL_H_ */

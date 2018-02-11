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

#include "FilePath.h"
#include "DbManager.h"
#include "Database.h"
#include "TextureAtlas.h"
#include "Layer.h"

namespace od
{

    class Level : public osg::Group, public AssetProvider
    {
    public:

        Level(const FilePath &levelPath, DbManager &dbManager);

        inline FilePath getFilePath() const { return mLevelPath; }

        // override osg::Group
        virtual const char *libraryName() const;
        virtual const char *className() const;

        // implement AssetProvider
        virtual TexturePtr getAssetAsTexture(const AssetRef &ref);
        virtual ModelPtr getAssetAsModel(const AssetRef &ref);
        virtual ClassPtr getAssetAsClass(const AssetRef &ref);


    private:

        void _loadLevel();
        void _loadNameAndDeps(SrscFile &file);
        void _loadLayers(SrscFile &file);
        void _loadLayerGroups(SrscFile &file);
        void _loadObjects(SrscFile &file);


        FilePath mLevelPath;
        DbManager &mDbManager;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::map<uint16_t, DbRefWrapper> mDependencyMap;
        std::vector<LayerPtr> mLayers;

        osg::ref_ptr<osg::Group> mLayerGroup;
        osg::ref_ptr<osg::Group> mObjectGroup;
    };


}

#endif /* RIOTLEVEL_H_ */

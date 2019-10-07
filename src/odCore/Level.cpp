/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */


#include <odCore/Level.h>

#include <algorithm>

#include <odCore/OdDefines.h>
#include <odCore/SrscRecordTypes.h>
#include <odCore/SrscFile.h>
#include <odCore/Logger.h>
#include <odCore/ZStream.h>
#include <odCore/Exception.h>
#include <odCore/LevelObject.h>
#include <odCore/BoundingBox.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace od
{

    Level::Level(odPhysics::PhysicsSystem &physicsSystem, odRender::Renderer *renderer)
    : mPhysicsSystem(physicsSystem)
    , mRenderer(renderer)
    , mMaxWidth(0)
    , mMaxHeight(0)
    , mVerticalExtent(0)
    , mCurrentActivePvsLayer(nullptr)
    {
    }

    Level::~Level()
    {
    	// despawn all remaining objects
    	for(auto it = mLevelObjects.begin(); it != mLevelObjects.end(); ++it)
    	{
    		(*it)->despawned();
    	}
    }

    void Level::loadLevel(const FilePath &levelPath, odDb::DbManager &dbManager)
    {
        Logger::info() << "Loading level " << levelPath.str();

        SrscFile file(levelPath);

        _loadNameAndDeps(file, dbManager);
        _loadLayers(file);
        //_loadLayerGroups(file); unnecessary, as this is probably just an editor thing
        _loadObjects(file);

        Logger::info() << "Level loaded successfully";
    }

    void Level::requestLevelObjectDestruction(LevelObject *obj)
    {
        mDestructionQueue.push_back(obj);
    }

    Layer *Level::getLayerById(uint32_t id)
    {
        auto pred = [id](std::unique_ptr<Layer> &l){ return l->getId() == id; };

        auto it = std::find_if(mLayers.begin(), mLayers.end(), pred);
        if(it == mLayers.end())
        {
            return nullptr;
        }

        return it->get();
    }

    Layer *Level::getLayerByIndex(uint16_t index)
    {
        if(index >= mLayers.size())
        {
            return nullptr;
        }

        return mLayers[index].get();
    }

    void Level::findAdjacentAndOverlappingLayers(Layer *checkLayer, std::vector<Layer*> &results)
    {
        // TODO: use an efficient spatial search here
        //  using brute force for now

        results.clear();

        float epsilon = 0.25;

        for(auto it = mLayers.begin(); it != mLayers.end(); ++it)
        {
            if(it->get() == checkLayer)
            {
                continue;
            }

            if((*it)->getBoundingBox().intersects(checkLayer->getBoundingBox(), epsilon))
            {
                results.push_back(it->get());
            }
        }
    }

    void Level::initialSpawn()
    {
        for(auto &obj : mLevelObjects)
        {
            if(obj->getSpawnStrategy() == SpawnStrategy::Always)
            {
                obj->spawned();
            }
        }
    }

    void Level::spawnAllObjects()
    {
        Logger::info() << "Spawning all objects for debugging (conditional spawning not implemented yet)";

        for(auto it = mLayers.begin(); it != mLayers.end(); ++it)
        {
            (*it)->spawn(mPhysicsSystem, mRenderer);
        }

        for(auto it = mLevelObjects.begin(); it != mLevelObjects.end(); ++it)
        {
            (*it)->spawned();
        }
    }

    void Level::update(float relTime)
    {
        if(!mDestructionQueue.empty())
        {
            auto it = mDestructionQueue.begin();
            while(it != mDestructionQueue.end())
            {
                (*it)->despawned();
                (*it)->destroyed();

                it = mDestructionQueue.erase(it);
            }
        }

        mTempObjectUpdateQueue.clear();
        mTempObjectUpdateQueue.assign(mObjectUpdateQueue.begin(), mObjectUpdateQueue.end());

        for(auto obj : mTempObjectUpdateQueue)
        {
            obj->update(relTime);
        }

        for(auto obj : mTempObjectUpdateQueue)
        {
            obj->postUpdate();
        }
    }

    LevelObject *Level::getLevelObjectByIndex(uint16_t index)
    {
        if(index >= mLevelObjects.size())
        {
            return nullptr;
        }

        return mLevelObjects[index].get();
    }

    LevelObject *Level::findObjectOfType(odRfl::ClassId id)
    {
        for(auto &obj : mLevelObjects)
        {
            if(obj->getClass() != nullptr && obj->getClass()->getRflClassId() == id)
            {
                return obj.get();
            }
        }

        return nullptr;
    }

    odDb::AssetProvider &Level::getDependency(uint16_t index)
    {
        auto it = mDependencyMap.find(index);
        if(it == mDependencyMap.end())
        {
            Logger::error() << "Database index " << index << " not found in level dependencies";
            throw NotFoundException("Database index not found in level dependencies");
        }

        return it->second;
    }

    void Level::activateLayerPVS(Layer *layer)
    {
        if(layer == nullptr)
        {
            for(auto &layer : mLayers)
            {
                layer->despawn();
            }

        }else if(mCurrentActivePvsLayer == nullptr)
        {
            for(auto index : layer->getVisibleLayerIndices())
            {
                getLayerByIndex(index)->spawn(mPhysicsSystem, mRenderer);
            }

        }else
        {
            // we want to despawn all layers that are in the previous layer's PVS but not in the passed layer's PVS,
            // while keeping all the ones that are in both. despawning all active layers and respawning the relevant
            // ones would be too inefficient

            auto &newPvs = layer->getVisibleLayerIndices();
            auto &oldPvs = mCurrentActivePvsLayer->getVisibleLayerIndices();

            std::vector<uint32_t> layerPvsMerged = newPvs;
            layerPvsMerged.insert(layerPvsMerged.end(), oldPvs.begin(), oldPvs.end());
            std::sort(layerPvsMerged.begin(), layerPvsMerged.end());

            for(size_t i = 0; i < layerPvsMerged.size()-1; ++i)
            {
                if(layerPvsMerged[i] == layerPvsMerged[i+1])
                {
                    // this layer stays
                    i++;
                    continue;

                }else
                {
                    // this layer changed
                    Layer *layerToToggle = getLayerByIndex(layerPvsMerged[i]);
                    if(layerToToggle->isSpawned())
                    {
                        layerToToggle->despawn();

                    }else
                    {
                        layerToToggle->spawn(mPhysicsSystem, mRenderer);
                    }
                }
            }
        }

        mCurrentActivePvsLayer = layer;
    }

    void Level::calculateInitialLayerAssociations()
    {
        // create temporary physics handles for all layers (without lighting, etc.)
        std::vector<od::RefPtr<odPhysics::LayerHandle>> layerHandles;
        layerHandles.reserve(mLayers.size());
        for(auto &layer : mLayers)
        {
            if(layer->isSpawned())
            {
                continue;
            }

            layerHandles.push_back(mPhysicsSystem.createLayerHandle(*layer));
        }

        for(auto &obj : mLevelObjects)
        {
            obj->updateAssociatedLayer(false);
        }
    }

    void Level::addToUpdateQueue(LevelObject &obj)
    {
        mObjectUpdateQueue.insert(&obj);
    }

    void Level::removeFromUpdateQueue(LevelObject &obj)
    {
        auto it = mObjectUpdateQueue.find(&obj);
        if(it != mObjectUpdateQueue.end())
        {
            mObjectUpdateQueue.erase(it);
        }
    }

    void Level::_loadNameAndDeps(SrscFile &file, odDb::DbManager &dbManager)
    {
        auto cursor = file.getFirstRecordOfType(SrscRecordType::LEVEL_NAME);
    	DataReader dr = cursor.getReader();

    	dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::verbose() << "Level depends on " << dbRefCount << " databases";

        for(size_t i = 0; i < dbRefCount; ++i)
        {
        	uint16_t dbIndex;
            dr  >> dbIndex
            	>> DataReader::Expect<uint16_t>(0);

            std::string dbPathStr;
            dr >> dbPathStr;

            FilePath dbPath(dbPathStr, file.getFilePath());
            odDb::Database &db = dbManager.loadDb(dbPath.adjustCase());

            Logger::debug() << "Level dependency index " << dbIndex << ": " << dbPath;

            mDependencyMap.insert(std::make_pair(dbIndex, std::ref(db)));
        }
    }

    void Level::_loadLayers(SrscFile &file)
    {
    	auto cursor = file.getFirstRecordOfType(SrscRecordType::LEVEL_LAYERS);
    	DataReader dr = cursor.getReader();

    	uint32_t layerCount;
    	dr >> layerCount;

    	mLayers.reserve(layerCount);

    	Logger::verbose() << "Level has " << layerCount << " layers";

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		std::unique_ptr<Layer> layer = std::make_unique<Layer>(*this);
    		layer->loadDefinition(dr);

    		mLayers.push_back(std::move(layer));
    	}

    	dr >> DataReader::Expect<uint32_t>(1);

    	float minHeight = std::numeric_limits<float>::max();
    	float maxHeight = std::numeric_limits<float>::lowest();

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		uint32_t compressedDataSize;
			dr >> compressedDataSize;

			ZStream zstr(dr.getStream(), compressedDataSize, ZStreamBuffer::DefaultBufferSize);
			DataReader zdr(zstr);
			mLayers[i]->loadPolyData(zdr);
			zstr.seekToEndOfZlib();

			if(mLayers[i]->getMinHeight() < minHeight)
			{
			    minHeight = mLayers[i]->getMinHeight();
			}

			if(mLayers[i]->getMaxHeight() > maxHeight)
			{
			    maxHeight = mLayers[i]->getMaxHeight();
			}
    	}

    	mVerticalExtent = maxHeight - minHeight;
    }

    void Level::_loadLayerGroups(SrscFile &file)
    {
    	auto cursor = file.getFirstRecordOfType(SrscRecordType::LEVEL_LAYERGROUPS);
        DataReader dr = cursor.getReader();

    	uint32_t groupCount;
    	dr >> groupCount;

    	for(size_t i = 0; i < groupCount; ++i)
    	{
    		std::string groupName;
    		uint32_t layerCount;

    		dr >> groupName
			   >> layerCount;

    		Logger::info() << "Group " << groupName << " has " << layerCount << " layers:";

    		for(size_t j = 0; j < layerCount; ++j)
    		{
    			uint32_t layer;
    			dr >> layer;
    		}

    	}
    }

    void Level::_loadObjects(SrscFile &file)
    {
    	Logger::verbose() << "Loading level objects";

    	auto cursor = file.getFirstRecordOfType(SrscRecordType::LEVEL_OBJECTS);
    	if(!cursor.isValid())
    	{
    		Logger::verbose() << "Level has no objects";
    		return;
    	}

        DataReader dr = cursor.getReader();

    	uint16_t objectCount;
    	dr >> objectCount;

    	Logger::verbose() << "Level has " << objectCount << " objects";

    	mLevelObjects.reserve(objectCount);
    	for(size_t i = 0; i < objectCount; ++i)
    	{
    		std::unique_ptr<od::LevelObject> object = std::make_unique<od::LevelObject>(*this);
    		object->loadFromRecord(dr);

    		mLevelObjects.push_back(std::move(object));
    	}
    }
}



/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */


#include <odCore/Level.h>

#include <algorithm>

#include <odCore/Client.h>
#include <odCore/SrscRecordTypes.h>
#include <odCore/SrscFile.h>
#include <odCore/Logger.h>
#include <odCore/ZStream.h>
#include <odCore/Panic.h>
#include <odCore/Layer.h>
#include <odCore/LevelObject.h>
#include <odCore/BoundingBox.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace od
{

    Level::Level(Engine engine)
    : mEngine(engine)
    , mPhysicsSystem(engine.getPhysicsSystem())
    , mRenderer(nullptr)
    , mMaxWidth(0)
    , mMaxHeight(0)
    , mDependencyTable(std::make_shared<odDb::DependencyTable>())
    , mVerticalExtent(0)
    , mCurrentActivePvsLayer(nullptr)
    {
        if(engine.isClient())
        {
            mRenderer = &engine.getClient().getRenderer();
        }
    }

    Level::~Level()
    {
    	// despawn all remaining objects
    	for(auto &objMap : mLevelObjects)
    	{
    		objMap.second->despawn();
    	}
    }

    void Level::loadLevel(const FilePath &levelPath, odDb::DbManager &dbManager)
    {
        Logger::info() << "Loading level " << levelPath.str();

        SrscFile file(levelPath);

        _loadNameAndDeps(file, dbManager);
        _loadLayers(file);
        //_loadLayerGroups(file); unnecessary, as this is probably just an editor thing
        _loadObjects(file, dbManager);

        Logger::info() << "Level loaded successfully";
    }

    void Level::addToDestructionQueue(LevelObjectId objId)
    {
        mDestructionQueue.insert(objId);
    }

    Layer *Level::getLayerById(uint32_t id)
    {
        // TODO: we can do better than O(n)
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
        for(auto &objMap : mLevelObjects)
        {
            auto &obj = objMap.second;
            if(obj->getSpawnStrategy() == SpawnStrategy::Always)
            {
                obj->spawn();
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

        for(auto &objMap : mLevelObjects)
        {
            objMap.second->spawn();
        }
    }

    void Level::update(float relTime)
    {
        if(!mDestructionQueue.empty())
        {
            for(auto objId : mDestructionQueue)
            {
                auto it = mLevelObjects.find(objId);
                if(it == mLevelObjects.end()) continue;

                it->second->despawn();

                mLevelObjects.erase(it);
            }
            mDestructionQueue.clear();
        }

        for(auto &objIt : mLevelObjects)
        {
            objIt.second->update(relTime);
        }

        for(auto &objIt : mLevelObjects)
        {
            objIt.second->postUpdate(relTime);
        }
    }

    ObjectRecordData &Level::getObjectRecord(uint16_t index)
    {
        if(index < 0 || index >= mObjectRecords.size())
        {
            OD_PANIC() << "Object index out of bounds";
        }

        return mObjectRecords[index];
    }

    std::shared_ptr<LevelObject> Level::getLevelObjectById(LevelObjectId id)
    {
        auto it = mLevelObjects.find(id);
        if(it == mLevelObjects.end())
        {
            return nullptr;
        }

        return it->second;
    }

    std::shared_ptr<LevelObject> Level::findFirstObjectOfType(odRfl::ClassId id)
    {
        for(auto &objMap : mLevelObjects)
        {
            auto &obj = objMap.second;
            if(obj->getClass() != nullptr && obj->getClass()->getRflClassId() == id)
            {
                return obj;
            }
        }

        return nullptr;
    }

    void Level::findObjectsOfType(odRfl::ClassId id, std::vector<std::shared_ptr<LevelObject>> &results)
    {
        for(auto &objMap : mLevelObjects)
        {
            auto &obj = objMap.second;
            if(obj->getClass() != nullptr && obj->getClass()->getRflClassId() == id)
            {
                results.push_back(obj);
            }
        }
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
        std::vector<std::shared_ptr<odPhysics::LayerHandle>> layerHandles;
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
            obj.second->updateAssociatedLayer(false);
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

            FilePath dbPath(dbPathStr, file.getFilePath().dir());

            Logger::debug() << "Gonna load level dependency index " << dbIndex << ": " << dbPath;
            auto db = dbManager.loadDatabase(dbPath.ext(".db").adjustCase());

            mDependencyTable->addDependency(dbIndex, db);
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

    void Level::_loadObjects(SrscFile &file, odDb::DbManager &dbManager)
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

        mObjectRecords.reserve(objectCount);
        for(size_t i = 0; i < objectCount; ++i)
    	{
            mObjectRecords.emplace_back(dr);
        }

    	mLevelObjects.reserve(objectCount);
        for(size_t i = 0; i < objectCount; ++i)
    	{
            auto &record = mObjectRecords[i];

            auto dbClass = mDependencyTable->loadAsset<odDb::Class>(record.getClassRef());
            if(dbClass == nullptr)
            {
                // ignore objects whose class we failed to load
                Logger::warn() << "Failed to load Class of object. Ignoring object";
                continue;
            }

            auto rflClassInstance = dbClass->makeInstance(mEngine);
            if(rflClassInstance == nullptr)
            {
                // ignore objects without a class implementation. they will sit around doing nothing anyways.
                continue;
            }

            auto newObject = std::make_unique<LevelObject>(*this, i, record, record.getObjectId(), dbClass);
            newObject->setRflClassInstance(std::move(rflClassInstance));

            auto &ptrInMap = mLevelObjects[record.getObjectId()];
            if(ptrInMap != nullptr)
            {
                OD_PANIC() << "Level contains non-unique object IDs (this might actually be an error in our level file interpretation)";
            }

            ptrInMap = std::move(newObject);
    	}
    }
}

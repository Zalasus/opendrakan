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
#include <odCore/Engine.h>
#include <odCore/LevelObject.h>
#include <odCore/BoundingBox.h>

#include <odCore/physics/PhysicsSystem.h>
#include <odCore/physics/Handles.h>

namespace od
{

    Level::Level(const FilePath &levelPath, Engine &engine)
    : mLevelPath(levelPath)
    , mEngine(engine)
    , mDbManager(engine.getDbManager())
    , mMaxWidth(0)
    , mMaxHeight(0)
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

    void Level::loadLevel()
    {
        Logger::info() << "Loading level " << mLevelPath.str();

        SrscFile file(mLevelPath);

        _loadNameAndDeps(file);
        _loadLayers(file);
        //_loadLayerGroups(file); unnecessary, as this is probably just an editor thing
        _loadObjects(file);

        for(auto &object : mLevelObjects)
        {
            object->buildLinks();
        }

        Logger::info() << "Level loaded successfully";
    }

    void Level::spawnAllObjects()
    {
        Logger::info() << "Spawning all objects for debugging (conditional spawning not implemented yet)";

        for(auto it = mLayers.begin(); it != mLayers.end(); ++it)
        {
            (*it)->spawn();
        }

        for(auto it = mLevelObjects.begin(); it != mLevelObjects.end(); ++it)
        {
            (*it)->spawned();
        }
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

    Layer *Level::getFirstLayerBelowPoint(const glm::vec3 &v)
    {
        // TODO: should we implement an efficient quadtree structure for spatial lookups of layers,
        //  we might want to switch to using that here. until then, we make the lookup using the physics system

        glm::vec3 end = v + glm::vec3(0, -1000, 0); // FIXME: we should make the ray length dynamic

        odPhysics::RayTestResult result;
        bool hasHit = mEngine.getPhysicsSystem().rayTestClosest(v, end, odPhysics::PhysicsTypeMasks::Layer, nullptr, result);

        if(hasHit)
        {
            odPhysics::LayerHandle *handle = result.handle->asLayerHandle();
            if(handle == nullptr)
            {
                throw od::Exception("Unexpected non-layer handle found during layer-below-object-search");
            }

            return &handle->getLayer();
        }

        return nullptr;
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

        for(auto it = mLevelObjects.begin(); it != mLevelObjects.end(); ++it)
        {
            (*it)->update(relTime);
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

    LevelObject *Level::findObjectOfType(odRfl::RflClassId id)
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

    void Level::_loadNameAndDeps(SrscFile &file)
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

            FilePath dbPath(dbPathStr, mLevelPath.dir());
            odDb::Database &db = mDbManager.loadDb(dbPath.adjustCase());

            Logger::debug() << "Level dependency index " << dbIndex << ": " << dbPath;

            mDependencyMap.insert(std::pair<uint16_t, odDb::DbRefWrapper>(dbIndex, db));
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

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		uint32_t compressedDataSize;
			dr >> compressedDataSize;

			ZStream zstr(dr.getStream(), compressedDataSize, ZStreamBuffer::DefaultBufferSize);
			DataReader zdr(zstr);
			mLayers[i]->loadPolyData(zdr);
			zstr.seekToEndOfZlib();
    	}
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



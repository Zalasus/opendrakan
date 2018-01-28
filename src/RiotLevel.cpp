/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#include "RiotLevel.h"

#include <algorithm>

#include "SrscFile.h"
#include "Logger.h"
#include "ZStream.h"

namespace od
{


	void Layer::loadDefinition(DataReader &dr)
	{
		dr 	>> mWidth
			>> mHeight;

		uint32_t type;
		dr >> type;
		mType = static_cast<LayerType>(type);

		dr  >> mOriginX
			>> mOriginY
			>> mWorldHeight
			>> mLayerName
			>> mFlags
			>> mLightDirection
			>> mLightAscension
			>> mLightColor
			>> mAmbientColor;

		uint32_t lightDropoffType;
		dr >> lightDropoffType;
		mLightDropoffType = static_cast<LightDropoffType>(lightDropoffType);

		uint32_t dummyLength;
		dr >> dummyLength;
		mDummyArray.reserve(dummyLength+1);
		for(size_t i = 0; i < dummyLength + 1; ++i)
		{
			uint32_t v;
			dr >> v;
			mDummyArray.push_back(v);
		}
	}

	void Layer::loadPolyData(DataReader &dr)
	{
		mVertices.reserve((mWidth+1)*(mHeight+1));
		for(size_t i = 0; i < mVertices.capacity(); ++i)
		{
			Vertex v;

			dr >> v.type;

			dr.ignore(1);

			uint16_t heightOffset;
			dr >> heightOffset;
			v.heightOffset = (heightOffset - 0x8000) * 2;

			mVertices.push_back(v);
		}

		mFaces.reserve(mWidth*mHeight);
		for(size_t i = 0; i < mFaces.capacity(); ++i)
		{
			Face f;

			uint16_t div;
			dr >> div;
			f.division = div ? Face::DIV_TOPLEFT_BOTTOMRIGHT : Face::DIV_BOTTOMLEFT_TOPRIGHT;

			dr >> f.textureLeft
			   >> f.textureRight;

			dr.ignore(16); // ignore texture orientation stuff
		}
	}



    RiotLevel::RiotLevel(const FilePath &levelPath, DbManager &dbManager)
    : mLevelPath(levelPath)
    , mDbManager(dbManager)
    , mMaxWidth(0)
    , mMaxHeight(0)
    {
        _loadLevel();
    }

    void RiotLevel::_loadLevel()
    {
    	Logger::info() << "Loading level " << mLevelPath.str();

        SrscFile file(mLevelPath);

        _loadNameAndDeps(file);
        _loadLayers(file);
        _loadLayerGroups(file);

        Logger::info() << "Level loaded successfully";
    }

    void RiotLevel::_loadNameAndDeps(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0000, 0));

    	dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::info() << "Level has " << dbRefCount << " dependencies:";

        for(size_t i = 0; i < dbRefCount; ++i)
        {

        	uint16_t dbIndex;
            dr  >> dbIndex;
            dr.ignore(2);

            std::string dbPathStr;
            dr >> dbPathStr;

            FilePath dbPath(dbPathStr, mLevelPath.dir());
            RiotDb &db = mDbManager.loadDb(dbPath.adjustCase());

            mDatabases[dbIndex] = &db;

            Logger::info() << "    " << dbIndex << ": " << dbPath.str();
        }
    }

    void RiotLevel::_loadLayers(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0001, 0));

    	uint32_t layerCount;
    	dr >> layerCount;

    	Logger::info() << "Level has " << layerCount << " layers";
    	mLayers.reserve(layerCount);

    	dr.ignore(4);

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		Logger::info() << "Loading layer " << i;

    		Layer layer;
    		layer.loadDefinition(dr);

    		mLayers.push_back(layer);
    	}

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		uint32_t zlibStuffSize;
			dr >> zlibStuffSize;

			size_t zlibOffset = dr.getStream().tellg();

			ZStream zstr(dr.getStream());
			DataReader zdr(zstr);
			mLayers[i].loadPolyData(zdr);
			zstr.seekToEndOfZlib();

			if((size_t)dr.getStream().tellg() != zlibOffset + zlibStuffSize)
			{
				throw Exception("ZStream read either too many or too few bytes");
			}
    	}
    }

    void RiotLevel::_loadLayerGroups(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordTypeId(0x0002, 0));

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

    			Logger::info() << "    " << std::hex << layer << std::dec;
    		}

    	}
    }

    void RiotLevel::_loadObjects(SrscFile &file)
    {
    	SrscFile::DirEntry objectRecordEntry;

    	try
    	{
    		objectRecordEntry = file.getDirectoryEntryByTypeAndID(0x0020, 0);

    	}catch(Exception &e)
    	{
    		// record not found -> this level has no objects
    		return;
    	}

    	DataReader dr(file.getStreamForRecord(objectRecordEntry));

    	uint16_t objectCount;
    	dr >> objectCount;

    	/*mObjects.reserve(objectCount);

    	for(size_t i = 0; i < objectCount; ++i)
    	{
    		uint32_t index;
    		dr >> index;

    		uint16_t classId;
    		uint16_t classDb;
    		dr >> classId
			   >> classDb;

    		dr.ignore(4);
    	}*/
    }
}



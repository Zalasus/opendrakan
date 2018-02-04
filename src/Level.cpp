/*
 * RiotLevel.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#include "Level.h"

#include <algorithm>
#include <osg/PolygonMode>
#include <osg/ShadeModel>
#include <osgUtil/SmoothingVisitor>
#include <osg/Texture2D>

#include "OdDefines.h"
#include "SrscRecordTypes.h"
#include "SrscFile.h"
#include "Logger.h"
#include "ZStream.h"
#include "Exception.h"

namespace od
{

	Layer::Layer(Level &level)
	: mLevel(level)
	, mId(0)
	, mWidth(0)
	, mHeight(0)
	, mType(TYPE_FLOOR)
	, mOriginX(0)
	, mOriginY(0)
	, mWorldHeight(0)
	, mLayerName("")
	, mFlags(0)
	, mLightDirection(0)
	, mLightAscension(0)
	, mLightColor(0)
	, mAmbientColor(0)
	, mLightDropoffType(DROPOFF_NONE)
	{
	}

	void Layer::loadDefinition(DataReader &dr)
	{
		dr 	>> mId
		    >> mWidth
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

		uint32_t visibleLayerCount;
		dr >> visibleLayerCount;
		mVisibleLayers.reserve(visibleLayerCount);
		for(size_t i = 0; i < visibleLayerCount; ++i)
		{
			uint32_t v;
			dr >> v;
			mVisibleLayers.push_back(v);
		}
	}

	void Layer::loadPolyData(DataReader &dr)
	{
		mGeometry = osg::ref_ptr<osg::Geometry>(new osg::Geometry());

        osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());
        vertices->reserve((mWidth+1)*(mHeight+1));
		for(size_t i = 0; i < (mWidth+1)*(mHeight+1); ++i)
		{
			size_t xRel = i % (mWidth + 1);
			size_t yRel = i / (mWidth + 1);

			Vertex v;

			dr >> v.type;

			dr.ignore(1);

			uint16_t heightOffset;
			dr >> heightOffset;
			v.heightOffset = (heightOffset - 0x8000) * 2;

			float x = mOriginX + xRel;
			float y = mOriginY + yRel;
			float z = 0.0005*(mWorldHeight + v.heightOffset);

			vertices->push_back(osg::Vec3(x,y,z));
		}
		mGeometry->setVertexArray(vertices);

		osg::ref_ptr<osg::DrawElementsUInt> primitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0));
		primitiveSet->reserve(mWidth*mHeight*6);
		for(size_t i = 0; i < mWidth*mHeight; ++i)
		{
			size_t faceY = i / mWidth;

			Face f;

			uint16_t div;
			dr >> div;
			f.division = div ? Face::DIV_TOPLEFT_BOTTOMRIGHT : Face::DIV_BOTTOMLEFT_TOPRIGHT;

			AssetRef textureLeftRef;
			AssetRef textureRightRef;

			dr >> textureLeftRef
			   >> textureRightRef;

			for(size_t i = 0; i < 8; ++i)
			{
			    uint16_t coord;
			    dr >> coord;
			}

			//  --a-----b--
			//    |     |
			//    |     |
			//  --c-----d--
			size_t a = i + faceY;
			size_t b = i + 1 + faceY;
			size_t c = i + mWidth + 1 + faceY;
			size_t d = i + mWidth + 2 + faceY;

			if(f.division == Face::DIV_BOTTOMLEFT_TOPRIGHT)
			{
				primitiveSet->push_back(a);
				primitiveSet->push_back(b);
				primitiveSet->push_back(c);

				primitiveSet->push_back(b);
				primitiveSet->push_back(d);
				primitiveSet->push_back(c);

			}else
			{
				primitiveSet->push_back(a);
				primitiveSet->push_back(b);
				primitiveSet->push_back(d);

				primitiveSet->push_back(a);
				primitiveSet->push_back(d);
				primitiveSet->push_back(c);
			}
		}
        mGeometry->addPrimitiveSet(primitiveSet);

        osg::ref_ptr<osg::Vec4Array> colorArray(new osg::Vec4Array());
        colorArray->push_back(osg::Vec4(0,0.7,0,0));
        mGeometry->setColorArray(colorArray);
        mGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

        this->addDrawable(mGeometry);

        osgUtil::SmoothingVisitor sm;
        this->accept(sm);
	}

	const char *Layer::libraryName() const
    {
        return OD_LIB_NAME;
    }

    const char *Layer::className() const
    {
        return "Layer";
    }



    Level::Level(const FilePath &levelPath, DbManager &dbManager)
    : mLevelPath(levelPath)
    , mDbManager(dbManager)
    , mMaxWidth(0)
    , mMaxHeight(0)
    {
        _loadLevel();
    }

    const char *Level::libraryName() const
    {
    	return OD_LIB_NAME;
    }

    const char *Level::className() const
    {
    	return "RiotLevel";
    }

    void Level::_loadLevel()
    {
    	Logger::info() << "Loading level " << mLevelPath.str();

        SrscFile file(mLevelPath);

        _loadNameAndDeps(file);
        _loadLayers(file);
        //_loadLayerGroups(file); unnecessary, as this is probably just an editor thing

        Logger::info() << "Level loaded successfully";
    }

    void Level::_loadNameAndDeps(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_NAME));

    	dr  >> mLevelName
            >> mMaxWidth
            >> mMaxHeight;

        uint32_t dbRefCount;
        dr >> dbRefCount;

        Logger::verbose() << "Level depends on " << dbRefCount << " databases";

        for(size_t i = 0; i < dbRefCount; ++i)
        {
        	uint16_t dbIndex;
            dr  >> dbIndex;
            dr.ignore(2);

            std::string dbPathStr;
            dr >> dbPathStr;

            FilePath dbPath(dbPathStr, mLevelPath.dir());
            Database &db = mDbManager.loadDb(dbPath.adjustCase());

            Logger::debug() << "Level dependency index " << dbIndex << ": " << dbPath;

            mDependencyMap.insert(std::pair<uint16_t, DbRefWrapper>(dbIndex, db));
        }
    }

    void Level::_loadLayers(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_LAYERS));

    	uint32_t layerCount;
    	dr >> layerCount;

    	mLayers.reserve(layerCount);

    	Logger::verbose() << "Level has " << layerCount << " layers";

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		Logger::verbose() << "Loading layer number " << i;

    		LayerPtr layer(new Layer(*this));
    		layer->loadDefinition(dr);

    		mLayers.push_back(layer);
    	}

    	dr.ignore(4);

    	for(size_t i = 0; i < layerCount; ++i)
    	{
    		uint32_t zlibStuffSize;
			dr >> zlibStuffSize;

			size_t zlibOffset = dr.getStream().tellg();

			ZStream zstr(dr.getStream());
			DataReader zdr(zstr);
			mLayers[i]->loadPolyData(zdr);
			zstr.seekToEndOfZlib();

			if((size_t)dr.getStream().tellg() != zlibOffset + zlibStuffSize)
			{
				throw IoException("ZStream read either too many or too few bytes");
			}

			this->addChild(mLayers[i].get());
    	}
    }

    void Level::_loadLayerGroups(SrscFile &file)
    {
    	DataReader dr(file.getStreamForRecordType(OD_SRSC_LEVEL_LAYERGROUPS));

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
    	SrscFile::DirIterator objectRecord = file.getDirIteratorByType(OD_SRSC_LEVEL_OBJECTS);
    	if(objectRecord == file.getDirectoryEnd())
    	{
    		return; // if record does not appear level has no objects
    	}

    	DataReader dr(file.getStreamForRecord(objectRecord));

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

    TexturePtr Level::getAssetAsTexture(const AssetRef &ref)
	{
    	Logger::debug() << "Requested asset " << ref.assetId << " from level dependency " << ref.dbIndex;

        auto it = mDependencyMap.find(ref.dbIndex);
        if(it == mDependencyMap.end())
        {
        	Logger::error() << "Database index " << ref.dbIndex << " not found in level dependencies";

            throw NotFoundException("Database index not found in level dependencies");
        }

        // TODO: instead of creating a new AssetRef everytime, why not add a getAsset method that just takes an ID?
        AssetRef foreignRef = ref;
        foreignRef.dbIndex = 0;
        return it->second.get().getAssetAsTexture(foreignRef);
    }
}



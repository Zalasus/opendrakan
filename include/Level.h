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


namespace od
{
	class Level;

	class Layer : public osg::Geode
	{
	public:

		enum LayerType
		{
			TYPE_FLOOR = 0,
			TYPE_CEILING = 1,
			TYPE_BETWEEN = 2
		};

		enum LightDropoffType
		{
			DROPOFF_NONE = 0,
			DROPOFF_N2S = 1,
			DROPOFF_E2W = 2,
			DROPOFF_S2N = 3,
			DROPOFF_W2E = 4
		};

		struct Vertex
		{
			uint8_t type;
			int32_t heightOffset; // this is the actual, signed offset, also scaled by 2!
		};

		struct Face
		{
			enum { DIV_BOTTOMLEFT_TOPRIGHT = 0, DIV_TOPLEFT_BOTTOMRIGHT = 1 } division;
			uint32_t textureLeft;
			uint32_t textureRight;
		};

		Layer(Level &level);

		void loadDefinition(DataReader &dr);
		void loadPolyData(DataReader &dr);

		inline uint32_t getId() const { return mId; };
		inline std::string getName() const { return mLayerName; };
		inline std::vector<uint32_t> &getVisibleLayers() { return mVisibleLayers; };

		// override osg::Geode
		virtual const char *libraryName() const;
		virtual const char *className() const;

	private:

		Level 			   &mLevel;
		uint32_t				mId;
		uint32_t 				mWidth;
		uint32_t 				mHeight;
		LayerType 				mType;
		uint32_t 				mOriginX;
		uint32_t 				mOriginZ;
		float 					mWorldHeight;
		std::string 			mLayerName;
		uint32_t 				mFlags; // 2 = member of alternate blending group
		float 					mLightDirection;
		float 					mLightAscension;
		uint32_t 				mLightColor;
		uint32_t 				mAmbientColor;
		LightDropoffType 		mLightDropoffType;
		std::vector<uint32_t>	mVisibleLayers;

		osg::ref_ptr<osg::Geometry> mGeometry;
	};


	typedef osg::ref_ptr<Layer> LayerPtr;


    class Level : public osg::Group, public AssetProvider
    {
    public:

        Level(const FilePath &levelPath, DbManager &dbManager);

        // override osg::Group
        virtual const char *libraryName() const;
        virtual const char *className() const;

        // implement AssetRefTranslator
        virtual TexturePtr getAssetAsTexture(const AssetRef &ref);


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
    };


}

#endif /* RIOTLEVEL_H_ */

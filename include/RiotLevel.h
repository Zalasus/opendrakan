/*
 * RiotLevel.h
 *
 *  Created on: 25 Jan 2018
 *      Author: zal
 */

#ifndef RIOTLEVEL_H_
#define RIOTLEVEL_H_

#include "FilePath.h"

#include <string>
#include <vector>
#include <map>

#include "RiotDb.h"
#include "DbManager.h"

namespace od
{

	class Layer
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

		void loadDefinition(DataReader &dr);
		void loadPolyData(DataReader &dr);

		inline std::vector<uint32_t> &getDummyArray() { return mDummyArray; };


	private:

		uint32_t 				mWidth;
		uint32_t 				mHeight;
		LayerType 				mType;
		uint32_t 				mOriginX;
		uint32_t 				mOriginY;
		float 					mWorldHeight;
		std::string 			mLayerName;
		uint32_t 				mFlags; // 2 = member of alternate blending group
		float 					mLightDirection;
		float 					mLightAscension;
		uint32_t 				mLightColor;
		uint32_t 				mAmbientColor;
		LightDropoffType 		mLightDropoffType;
		std::vector<uint32_t>	mDummyArray;
		std::vector<Vertex>   	mVertices;
		std::vector<Face>	  	mFaces;
	};


    class RiotLevel
    {
    public:

        RiotLevel(const FilePath &levelPath, DbManager &dbManager);




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
        std::map<uint16_t, RiotDb*> mDatabases;
        std::vector<Layer> mLayers;
    };


}

#endif /* RIOTLEVEL_H_ */

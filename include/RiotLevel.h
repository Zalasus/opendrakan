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

#include "SimpleArray.h"
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

		void load(DataReader &dr);

	private:

		uint32_t width;
		uint32_t height;
		uint32_t type;
		uint32_t origin_x;
		uint32_t origin_y;
		float world_height;
		std::string layer_name;
		uint32_t flags; // 2 = member of alternate blending group
		float light_direction;
		float light_ascension;
		uint32_t light_color;
		uint32_t ambient_color;
		uint32_t light_dropoff_type;
		uint32_t dummyLength;
		//uint32      ???;
		//uint32[~1]	???;
	};


    class RiotLevel
    {
    public:

        RiotLevel(const FilePath &levelPath, DbManager &dbManager);


    private:

        struct DbRef
        {
            RiotDb *db;
            uint16_t index;
        };

        void _loadLevel();
        void _loadNameAndDeps(SrscFile &file);
        void _loadLayers(SrscFile &file);


        FilePath mLevelPath;
        DbManager &mDbManager;

        std::string mLevelName;
        uint32_t mMaxWidth;
        uint32_t mMaxHeight;
        std::vector<DbRef> mDatabases;
        std::vector<Layer> mLayers;
    };


}

#endif /* RIOTLEVEL_H_ */

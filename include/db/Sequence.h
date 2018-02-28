/*
 * Sequence.h
 *
 *  Created on: 28 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_DB_SEQUENCE_H_
#define INCLUDE_DB_SEQUENCE_H_

#include <osg/Quat>

#include "db/Asset.h"

namespace od
{

	class Sequence : public Asset
	{
	public:

		enum ActionType
		{
			TRANSFORM 	= 0,
			START_ANIM 	= 1,
			PLAY_SOUND 	= 3,
			ATTACH 		= 4,
			RUN_STOP_AI = 5,
			SHOW_HIDE   = 6,
			MESSAGE     = 7
		};

		enum InterpolationType
		{
			NONE = 0,
			LINEAR_LINEAR = 0x10,
			LINEAR_SPLINE = 0x20,
			SPLINE_SPLINE = 0x30
		};

		struct Action
		{
			ActionType type;
			float timeOffset;
		};

		struct ActionTransform : Action
		{
			osg::Quat  rotation;
			osg::Vec3f position;
		};

		struct Actor
		{
			std::string name;
			uint32_t actorId;
			uint32_t levelObjectId;
			std::vector<Action> actions;
		};

		void loadSequenceRecord(DataReader dr);

		virtual const char *getAssetTypeName() const override { return "sequence"; }


	private:

		std::string mSequenceName;

	};

}

#endif /* INCLUDE_DB_SEQUENCE_H_ */

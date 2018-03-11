/*
 * SrscRecordTypes.h
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_SRSCRECORDTYPES_H_
#define INCLUDE_SRSCRECORDTYPES_H_

namespace od
{

	enum class SrscRecordType : uint16_t
	{
		LEVEL_NAME 			= 0x0000,
		LEVEL_LAYERS		= 0x0001,
		LEVEL_LAYERGROUPS	= 0x0002,
		LEVEL_OBJECTS		= 0x0020,

		PALETTE 			= 0x0030,
		TEXTURE	 			= 0x0040,

		CLASS_RFL           = 0x0100,
		CLASS_GROUP         = 0x0101,
		CLASS               = 0x0102,

		MODEL_NAME          = 0x0200,
		MODEL_GROUP         = 0x0201,
		MODEL_UNKNOWN1      = 0x0202,
		MODEL_VERTICES      = 0x0203,
		MODEL_FACES         = 0x0204,
		MODEL_TEXTURES      = 0x0206,
		MODEL_BOUNDING      = 0x0207,
		MODEL_LOD_BONES     = 0x0208,
		MODEL_UNKNOWN2      = 0x0209,

		SEQUENCE_UNKNOWN1   = 0x0310,
		SEQUENCE            = 0x0311,
		SEQUENCE_GROUP      = 0x0312
	};

}

#endif /* INCLUDE_SRSCRECORDTYPES_H_ */

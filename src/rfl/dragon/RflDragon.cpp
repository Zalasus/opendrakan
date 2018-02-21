/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */


#include "rfl/Rfl.h"
#include "rfl/general/RflMaterial.h"
#include "rfl/general/RflCamera.h"
//#include "rfl/general/RflPlayerControl.h"
#include "rfl/dragon/RflHealthPack.h"
#include "rfl/dragon/RflKey.h"

namespace od
{

	OD_REGISTER_RFL("Dragon");

	// TODO: move the following in their respective cpp files wherever possible

	// Item
	OD_REGISTER_RFL_CLASS(0x000A, "Health Pack", RflHealthPack);
	OD_REGISTER_RFL_CLASS(0x0021, "Key",         RflKey);

	// Material
	OD_REGISTER_RFL_CLASS(0x004b, "Standard Material", RflMaterial);
	OD_REGISTER_RFL_CLASS(0x0072, "Blended Material",  RflBlendedMaterial);

	// Player
	//OD_REGISTER_RFL_CLASS(0x0009, "Player Control", RflPlayerControl);

	// System
	OD_REGISTER_RFL_CLASS(0x001b, "Tracking Camera",   RflTrackingCamera);

}

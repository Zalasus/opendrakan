/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */


#include "rfl/RiotFunctionLibrary.h"

#include "rfl/RflHealthPack.h"
#include "rfl/RflKey.h"
#include "rfl/RflMaterial.h"

namespace od
{

	OD_REGISTER_RFL("Dragon");
	OD_REGISTER_RFL_CLASS(0x000A, "Health Pack", RflHealthPack);
	OD_REGISTER_RFL_CLASS(0x0021, "Key",         RflKey);

	// Material
	OD_REGISTER_RFL_CLASS(0x004b, "Standard Material", RflMaterial);
	OD_REGISTER_RFL_CLASS(0x0072, "Blended Material",  RflBlendedMaterial);

}

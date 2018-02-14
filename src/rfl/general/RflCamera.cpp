/*
 * RflCamera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/general/RflCamera.h"

namespace od
{

	RflTrackingCamera::RflTrackingCamera(RflFieldProbe &probe)
	: RflClass(probe)
	, mTrackingMode(probe, "Tracking Mode", 1)
	, mRubberBandStrength(probe, "Rubber Band Strength", 2)
	, mSpinSpeed(probe, "Spin Speed", 20)
	, mCrosshairDistance(probe, "Cross-hair Distance (lu)", 8)
	{
	}

}

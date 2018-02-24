/*
 * RflCamera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/general/RflCamera.h"

#include "rfl/Rfl.h"

namespace od
{

	RflTrackingCamera::RflTrackingCamera()
	: mTrackingMode(1)
	, mRubberBandStrength(2)
	, mSpinSpeed(20)
	, mCrosshairDistance(8)
	{
	}

	void RflTrackingCamera::probeFields(RflFieldProbe &probe)
	{
	    probe.beginCategory("Camera Options");
	    probe.registerField(mTrackingMode, "Tracking Mode");
	    probe.registerField(mRubberBandStrength, "Rubber Band Strength");
	    probe.registerField(mSpinSpeed, "Spin Speed");
	    probe.registerField(mCrosshairDistance, "Cross-hair Distance (lu)");
	}

	OD_REGISTER_RFL_CLASS(0x001b, "Tracking Camera", RflTrackingCamera);
}

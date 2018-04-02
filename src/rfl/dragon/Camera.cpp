/*
 * Camera.cpp
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#include "rfl/dragon/Camera.h"

#include "rfl/Rfl.h"
#include "Level.h"
#include "Engine.h"

namespace odRfl
{

	TrackingCamera::TrackingCamera()
	: mTrackingMode(1)
	, mRubberBandStrength(2)
	, mSpinSpeed(20)
	, mCrosshairDistance(8)
	{
	}

	void TrackingCamera::probeFields(RflFieldProbe &probe)
	{
	    probe.beginCategory("Camera Options");
	    probe.registerField(mTrackingMode, "Tracking Mode");
	    probe.registerField(mRubberBandStrength, "Rubber Band Strength");
	    probe.registerField(mSpinSpeed, "Spin Speed");
	    probe.registerField(mCrosshairDistance, "Cross-hair Distance (lu)");
	}

	void TrackingCamera::spawn(od::LevelObject &obj)
	{
	}

	OD_REGISTER_RFL_CLASS(0x001b, "Tracking Camera", TrackingCamera);
}

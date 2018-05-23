/*
 * Camera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_CAMERA_H_
#define INCLUDE_RFL_DRAGON_CAMERA_H_

#include "rfl/RflClass.h"
#include "rfl/RflField.h"

namespace odRfl
{

	class TrackingCamera : public RflClass
	{
	public:

		TrackingCamera();

		virtual void probeFields(RflFieldProbe &probe) override;
		virtual void spawned(od::LevelObject &obj) override;


	private:

		RflEnum		mTrackingMode; // 0 = Cockpit, 1 = Rubber Band, 2 = Chase Fixed
		RflFloat    mRubberBandStrength;
		RflFloat    mSpinSpeed;
		RflFloat	mCrosshairDistance;

	};

}

#endif /* INCLUDE_RFL_DRAGON_CAMERA_H_ */

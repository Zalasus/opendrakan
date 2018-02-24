/*
 * RflCamera.h
 *
 *  Created on: 14 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_GENERAL_RFLCAMERA_H_
#define INCLUDE_RFL_GENERAL_RFLCAMERA_H_

#include "rfl/RflClass.h"

namespace od
{

	class RflTrackingCamera : public RflClass
	{
	public:

		RflTrackingCamera();

		virtual void probeFields(RflFieldProbe &probe) override;


	private:

		RflEnum		mTrackingMode;
		RflFloat    mRubberBandStrength;
		RflFloat    mSpinSpeed;
		RflFloat	mCrosshairDistance;
	};

}

#endif /* INCLUDE_RFL_GENERAL_RFLCAMERA_H_ */

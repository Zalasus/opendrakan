/*
 * Sky.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */


#include "rfl/dragon/Sky.h"

#include <osg/Depth>

#include "rfl/Rfl.h"
#include "Level.h"
#include "OdDefines.h"
#include "Engine.h"
#include "Camera.h"

namespace odRfl
{

	class SkyUpdateCallback : public osg::NodeCallback
	{
	public:

		SkyUpdateCallback(DomedSky &sky)
		: mSky(sky)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			// traverse first, in case the sky object is animated for some reason
			traverse(node, nv);

			mSky.update(nv->getEyePoint());
		}


	private:

		DomedSky &mSky;

	};



	DomedSky::DomedSky()
	: mPrimarySky(1) // yes
	, mFollowMode(0) // original height
	, mOffsetDown(10000.0)
	, mEffects(0) // none
	, mLightningObject(od::AssetRef::NULL_REF)
	, mAveLightningPeriod(5.0)
	, mLightningHeight(30)
	, mMinLightningDist(10)
	, mMaxLightningDist(20)
	, mLightningWedgeAngle(90.0)
	, mLensFlare(od::AssetRef::NULL_REF)
	, mFlareElevation(0)
	, mFlareDirection(0)
	{
	}

    void DomedSky::probeFields(RflFieldProbe &probe)
    {
    	/*probe.beginCategory("Position");
    	probe.registerField(mPrimarySky, "Primary Sky");
		probe.registerField(mFollowMode, "Follow Mode");
		probe.registerField(mOffsetDown, "Offset Down");

		probe.beginCategory("Effects");
		probe.registerField(mEffects, "Effects");
		probe.registerField(mLightningObject, "Lightning Object");
		probe.registerField(mAveLightningPeriod, "Ave Lightning Period");
		probe.registerField(mLightningHeight, "Lightning Height");
		probe.registerField(mMinLightningDist, "Min Lightning Dist");
		probe.registerField(mMaxLightningDist, "Max Lightning Dist");
		probe.registerField(mLightningWedgeAngle, "Lightning Wedge Angle");
		probe.registerField(mLensFlare, "Lens Flare");
		probe.registerField(mFlareElevation, "Flare Elevation (0 - 90)");
		probe.registerField(mFlareDirection, "Flare Direction (0 - 359)");*/

		// not really clean code, but avoids the wall of text above. should we use this rather than calling the methods directly?
		probe("Position")
    		 (mPrimarySky, "Primary Sky")
			 (mFollowMode, "Follow Mode")
			 (mOffsetDown, "Offset Down")

			 ("Effects")
			 (mEffects, "Effects")
			 (mLightningObject, "Lightning Object")
		     (mAveLightningPeriod, "Ave Lightning Period")
		     (mLightningHeight, "Lightning Height")
		     (mMinLightningDist, "Min Lightning Dist")
		     (mMaxLightningDist, "Max Lightning Dist")
		     (mLightningWedgeAngle, "Lightning Wedge Angle")
		     (mLensFlare, "Lens Flare")
		     (mFlareElevation, "Flare Elevation (0 - 90)")
		     (mFlareDirection, "Flare Direction (0 - 359)");
    }

    void DomedSky::spawned(od::LevelObject &obj)
	{
    	mSkyObject = &obj;

    	osg::StateSet *ss = mSkyObject->getOrCreateStateSet();
    	ss->setRenderBinDetails(-1, "RenderBin");
    	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ss->setAttributeAndModes(depth, osg::StateAttribute::ON);

		mSkyObject->addUpdateCallback(new SkyUpdateCallback(*this));
	}

    void DomedSky::update(osg::Vec3 eyePoint)
    {
    	osg::Vec3 newSkyPos = mSkyObject->getLevel().getEngine().getCamera().getEyePoint();
    	newSkyPos.y() -= mOffsetDown * OD_WORLD_SCALE;

    	mSkyObject->setPosition(newSkyPos);
    }

    OD_REGISTER_RFL_CLASS(0x001a, "Domed Sky", DomedSky);

}

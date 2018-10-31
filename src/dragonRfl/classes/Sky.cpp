/*
 * Sky.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */


#include <dragonRfl/classes/Sky.h>

#include <osg/Depth>
#include <dragonRfl/RflDragon.h>
#include <odCore/rfl/Rfl.h>
#include <odCore/Level.h>
#include <odCore/OdDefines.h>
#include <odCore/Engine.h>
#include <odCore/Camera.h>
#include <odCore/LevelObject.h>

namespace dragonRfl
{

	DomedSky::DomedSky(DragonRfl &rfl)
	: mPrimarySky(true)
	, mFollowMode(0) // original height
	, mOffsetDown(10000.0)
	, mEffects(0) // none
	, mLightningObject(odDb::AssetRef::NULL_REF)
	, mAveLightningPeriod(5.0)
	, mLightningHeight(30)
	, mMinLightningDist(10)
	, mMaxLightningDist(20)
	, mLightningWedgeAngle(90.0)
	, mLensFlare(odDb::AssetRef::NULL_REF)
	, mFlareElevation(0)
	, mFlareDirection(0)
	{
	}

    void DomedSky::probeFields(odRfl::FieldProbe &probe)
    {
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

    void DomedSky::onSpawned(od::LevelObject &obj)
	{
        if(obj.getCachedNode() == nullptr)
        {
            return;
        }

    	osg::StateSet *ss = obj.getCachedNode()->getOrCreateStateSet();
    	ss->setRenderBinDetails(-1, "RenderBin");
    	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask(false);
		ss->setAttributeAndModes(depth, osg::StateAttribute::ON);

		od::LevelObject &camObject = obj.getLevel().getEngine().getCamera()->getLevelObject();
		osg::Vec3 newSkyPos = camObject.getPosition();
        newSkyPos.y() -= mOffsetDown * OD_WORLD_SCALE;
        obj.setPosition(newSkyPos);
		obj.attachTo(&camObject, false, true, false);
	}


    OD_REGISTER_RFLCLASS(DragonRfl, DomedSky);

}

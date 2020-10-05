/*
 * Sky.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Sky.h>

#include <odCore/Client.h>
#include <odCore/Level.h>
#include <odCore/LevelObject.h>
#include <odCore/Units.h>

#include <odCore/render/Renderer.h>

#include <dragonRfl/classes/TrackingCamera.h>

namespace dragonRfl
{

	DomedSkyFields::DomedSkyFields()
	: primarySky(true)
	, followMode(0) // original height
	, offsetDown(10000.0)
	, effects(0) // none
	, lightningObject(odDb::AssetRef::NULL_REF)
	, aveLightningPeriod(5.0)
	, lightningHeight(30)
	, minLightningDist(10)
	, maxLightningDist(20)
	, lightningWedgeAngle(90.0)
	, lensFlare(odDb::AssetRef::NULL_REF)
	, flareElevation(0)
	, flareDirection(0)
	{
	}

    void DomedSkyFields::probeFields(odRfl::FieldProbe &probe)
    {
		probe("Position")
    		     (primarySky, "Primary Sky")
			     (followMode, "Follow Mode")
			     (offsetDown, "Offset Down")
			 ("Effects")
			     (effects, "Effects")
			     (lightningObject, "Lightning Object")
			     (aveLightningPeriod, "Ave Lightning Period")
			     (lightningHeight, "Lightning Height")
			     (minLightningDist, "Min Lightning Dist")
			     (maxLightningDist, "Max Lightning Dist")
			     (lightningWedgeAngle, "Lightning Wedge Angle")
			     (lensFlare, "Lens Flare")
			     (flareElevation, "Flare Elevation (0 - 90)")
			     (flareDirection, "Flare Direction (0 - 359)");
    }


    DomedSky_Cl::DomedSky_Cl()
    : mCameraObject(nullptr)
    {
    }

    void DomedSky_Cl::onSpawned()
	{
        od::LevelObject &obj = getLevelObject();

        // handle attachment: attach sky to camera (not player!!!) FIXME: somehow, the sky is still a bit stuttery
        odRfl::ClassId cameraClassId = TrackingCamera::classId();
        mCameraObject = obj.getLevel().findFirstObjectOfType(cameraClassId);
        if(mCameraObject == nullptr)
        {
            // no camera present in level. we don't want a dangling sky in the level, so we let the sky commit suicide
            Logger::warn() << "Sky found no camera in the level. Destroying sky...";
            obj.requestDestruction();
            return;
        }

        // we need a special render node for the sky (in it's own render bin)
        mRenderNode = getClient().getRenderer().createHandleFromObject(obj);

        std::lock_guard<std::mutex> lock(mRenderNode->getMutex());
        mRenderNode->setRenderBin(odRender::RenderBin::SKY);

        obj.setEnableUpdate(true);
	}

    void DomedSky_Cl::onPostUpdate(float relTime)
    {
        if(mCameraObject != nullptr && mRenderNode != nullptr)
        {
            // only move render handle, not the object. this avoids unnecessary state changes
            glm::vec3 skyOffset(0, -od::Units::worldUnitsToLengthUnits(mFields.offsetDown.get()), 0);
            glm::vec3 skyPosition = mCameraObject->getPosition() + skyOffset;
            mRenderNode->setPosition(skyPosition);
        }
    }

}

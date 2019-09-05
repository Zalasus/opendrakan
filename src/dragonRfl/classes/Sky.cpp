/*
 * Sky.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include <dragonRfl/classes/Sky.h>

#include <odCore/Level.h>
#include <odCore/OdDefines.h>
#include <odCore/Engine.h>
#include <odCore/LevelObject.h>

#include <odCore/rfl/Rfl.h>

#include <odCore/render/Renderer.h>

#include <dragonRfl/RflDragon.h>

#include <dragonRfl/classes/TrackingCamera.h>

namespace dragonRfl
{

	DomedSky::DomedSky(DragonRfl &rfl, od::LevelObject &obj)
	: odRfl::LevelObjectClassBase(rfl, obj)
    , mPrimarySky(true)
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

    void DomedSky::onSpawned()
	{
        od::LevelObject &obj = getLevelObject();

        // handle attachment: attach sky to camera (not player!!!) FIXME: somehow, the sky is still a bit stuttery
        odRfl::ClassId cameraClassId = odRfl::ClassTraits<TrackingCamera>::classId();
        od::LevelObject *cameraObject = obj.getLevel().findObjectOfType(cameraClassId);
        if(cameraObject == nullptr)
        {
            // no camera present in level. we don't want a dangling sky in the level, so we let the sky commit suicide
            Logger::warn() << "Sky found no camera in the level. Destroying sky...";
            obj.requestDestruction();
            return;
        }

        glm::vec3 skyOffset(0, -mOffsetDown.get()*OD_WORLD_SCALE, 0);
        obj.setPosition(cameraObject->getPosition() + skyOffset);
        obj.attachTo(cameraObject, false, true, false);


        // we need a special render node for the sky (in it's own render bin)
        odRender::Renderer *renderer = obj.getLevel().getEngine().getRenderer();
        if(renderer == nullptr)
        {
            return;
        }

        mRenderNode = renderer->createHandleFromObject(obj);

        std::lock_guard<std::mutex> lock(mRenderNode->getMutex());
        mRenderNode->setRenderBin(odRender::RenderBin::SKY);
	}

    void DomedSky::onTranslated(const glm::vec3 &from, const glm::vec3 &to)
    {
        if(mRenderNode != nullptr)
        {
            std::lock_guard<std::mutex> lock(mRenderNode->getMutex());
            mRenderNode->setPosition(to);
        }
    }

}

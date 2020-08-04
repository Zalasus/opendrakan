/*
 * CrystalRingButton.cpp
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#include <dragonRfl/gui/CrystalRingButton.h>

#include <glm/gtc/matrix_transform.hpp>

#include <odCore/Exception.h>
#include <odCore/Client.h>

#include <odCore/render/Renderer.h>

#include <odCore/audio/SoundSystem.h>
#include <odCore/audio/Source.h>

#include <dragonRfl/gui/DragonGui.h>

// max crystal speed in rad/s
#define OD_CRYSTAL_SPEED_MAX       (3.0*M_PI)

// time a crystal needs to speed up from stop to max speed
#define OD_CRYSTAL_SPEED_RISETIME   3.0

// time a crystal at max speed needs to decelerate to stop
#define OD_CRYSTAL_SPEED_FALLTIME   3.0

// time a ring need to perform a full rotation
#define OD_RING_SPEED 1.0

namespace dragonRfl
{

    CrystalRingButton::CrystalRingButton(DragonGui &gui, std::shared_ptr<odDb::Model> crystalModel,
                                      std::shared_ptr<odDb::Model> innerRingModel,
                                      std::shared_ptr<odDb::Model> outerRingModel,
                                      std::shared_ptr<odDb::Sound> hoverSound, float noteOffset)
    : odGui::Widget(gui)
    , mCrystalColorInactive(0.38431, 0.36471, 0.54902, 1.0)
    , mCrystalColorActive(0.95686275, 0.25882353, 0.63137255, 1.0)
    , mRingColorInactive(1, 1, 1, 1)
    , mRingColorActive(1, 1, 1, 1)
    , mCallbackUserData(-1)
    , mCrystalSpeed(0.0)
    , mClicked(false)
    , mRingAnimPercent(0.0)
    , mCrystalColor(mCrystalColorInactive)
    , mRingColor(mRingColorInactive)
    {
        // select whatever model is not null for bounds calculation, starting with outer ring
        std::shared_ptr<odDb::Model> modelForBounds =
                (outerRingModel != nullptr) ? outerRingModel : ((innerRingModel != nullptr) ? innerRingModel : crystalModel);

        if(modelForBounds == nullptr)
        {
            throw od::Exception("Passed no non-null models to CrystalRingButton");
        }

        od::AxisAlignedBoundingBox aabb = modelForBounds->getCalculatedBoundingBox();
        float diameter = aabb.max().x - aabb.min().x;

        //glm::vec3 extends = aabb.max() - aabb.min();
        //float aspectRatio = extends.x/extends.y;
        //float fovDegrees = 70;
        //getRenderNode()->setPerspectiveMode(fovDegrees * M_PI/180, aspectRatio);

        glm::vec3 lightDiffuse(1.0, 1.0, 1.0);
        glm::vec3 lightAmbient(0.15, 0.15, 0.15);
        glm::vec3 lightDirection(glm::normalize(glm::vec3(0.0, 1.0, 1.0)));

        if(crystalModel != nullptr)
        {
            std::shared_ptr<odRender::Model> mn = getGui().getRenderer().getOrCreateModelFromDb(crystalModel);

            mCrystalHandle = getGui().getRenderer().createHandle(odRender::RenderSpace::GUI);

            mCrystalHandle->setModel(mn);
            mCrystalHandle->setScale(glm::vec3(0.58/diameter));
            mCrystalHandle->setPosition(glm::vec3(0.5, 0.5, 0));
            mCrystalHandle->setGlobalLight(lightDirection, lightDiffuse, lightAmbient);
            mCrystalHandle->setEnableColorModifier(true);
            mCrystalHandle->setColorModifier(mCrystalColor);

            this->addRenderHandle(mCrystalHandle);
        }

        if(innerRingModel != nullptr)
        {
            std::shared_ptr<odRender::Model> mn = getGui().getRenderer().getOrCreateModelFromDb(innerRingModel);

            mInnerRingHandle = getGui().getRenderer().createHandle(odRender::RenderSpace::GUI);

            mInnerRingHandle->setModel(mn);
            mInnerRingHandle->setScale(glm::vec3(1/diameter));
            mInnerRingHandle->setPosition(glm::vec3(0.5, 0.5, 0));
            mInnerRingHandle->setGlobalLight(lightDirection, lightDiffuse, lightAmbient);
            mInnerRingHandle->setEnableColorModifier(true);
            mInnerRingHandle->setColorModifier(mRingColor);

            this->addRenderHandle(mInnerRingHandle);
        }

        if(outerRingModel != nullptr)
        {
            std::shared_ptr<odRender::Model> mn = getGui().getRenderer().getOrCreateModelFromDb(outerRingModel);

            mOuterRingHandle = getGui().getRenderer().createHandle(odRender::RenderSpace::GUI);

            mOuterRingHandle->setModel(mn);
            mOuterRingHandle->setScale(glm::vec3(1/diameter));
            mOuterRingHandle->setPosition(glm::vec3(0.5, 0.5, 0));
            mOuterRingHandle->setGlobalLight(lightDirection, lightDiffuse, lightAmbient);
            mOuterRingHandle->setEnableColorModifier(true);
            mOuterRingHandle->setColorModifier(mRingColor);

            this->addRenderHandle(mOuterRingHandle);
        }

        if(hoverSound != nullptr && gui.getClient().getSoundSystem() != nullptr)
        {
            mSoundSource = gui.getClient().getSoundSystem()->createSource();
            mSoundSource->setPosition(glm::vec3(0.0));
            mSoundSource->setRelative(true);
            mSoundSource->setLooping(true);
            mSoundSource->setSound(hoverSound);

            // calculate pitch from note offset
            static const float halfTonePitch = std::pow(2.0, 1.0/12.0); // (assume an equal temperament scale)
            // the original's pitch offset is sensitive to the sound's sample rate while our sound system's is not.
            //  we replicate this behaviour by factoring in the ratio to the original, hardcoded rate
            float resamplingFactor = hoverSound->getSamplingFrequency() / 22050; // TODO: maybe move this to the Source class. it may be universal to all pitch shifting
            float soundPitch = std::pow(halfTonePitch, noteOffset) / resamplingFactor;
            mSoundSource->setPitch(soundPitch);
        }
    }

    CrystalRingButton::~CrystalRingButton()
    {
    }

    bool CrystalRingButton::liesWithinLogicalArea(const glm::vec2 &pos)
    {
        // logical area is circle around (0.5 0.5) TODO: implement non-circular buttons here
        glm::vec2 p = pos - glm::vec2(0.5, 0.5);
        return glm::length(p) <= 0.5;
    }

    void CrystalRingButton::onMouseDown(const glm::vec2 &pos, int button)
    {
        mClicked = true;
    }

    void CrystalRingButton::onMouseEnter(const glm::vec2 &pos)
    {
        if(mSoundSource != nullptr)
        {
            mSoundSource->play(1.0f);
        }

        mCrystalColor.move(mCrystalColorActive, 0.5);
        mRingColor.move(mRingColorActive, 0.5);

        mCrystalSpeed.move(OD_CRYSTAL_SPEED_MAX, OD_CRYSTAL_SPEED_RISETIME);
    }

    void CrystalRingButton::onMouseLeave(const glm::vec2 &pos)
    {
        if(mSoundSource != nullptr)
        {
            mSoundSource->stop(1.8f);
        }

        mCrystalColor.move(mCrystalColorInactive, 1.0);
        mRingColor.move(mRingColorInactive, 1.0);

        mCrystalSpeed.move(0, OD_CRYSTAL_SPEED_FALLTIME);
    }

    void CrystalRingButton::onUpdate(float relTime)
    {
        if(mClicked)
        {
            if(mRingAnimPercent < 1.0)
            {
                mRingAnimPercent = std::min(mRingAnimPercent + relTime/OD_RING_SPEED, 1.0);

            }else
            {
                // ring animation finished
                mClicked = false;
                mRingAnimPercent = 0.0;

                if(mClickedCallback)
                {
                    mClickedCallback(mCallbackUserData);
                }
            }
        }

        if(mCrystalHandle != nullptr)
        {
            mCrystalSpeed.update(relTime);

            // apply rotation to crystal
            glm::quat q = mCrystalHandle->getOrientation();
            q *= glm::quat(glm::vec3(0, -mCrystalSpeed*relTime, 0));
            mCrystalHandle->setOrientation(q);

            if(mCrystalColor.update(relTime))
            {
                mCrystalHandle->setColorModifier(mCrystalColor);
            }
        }

        if(mRingColor.update(relTime))
        {
            if(mOuterRingHandle != nullptr)
            {
                mOuterRingHandle->setColorModifier(mRingColor);
            }

            if(mInnerRingHandle != nullptr)
            {
                mInnerRingHandle->setColorModifier(mRingColor);
            }
        }

        if(mOuterRingHandle != nullptr)
        {
            glm::quat q(glm::vec3(mRingAnimPercent*M_PI, 0, 0));
            mOuterRingHandle->setOrientation(q);
        }

        if(mInnerRingHandle != nullptr)
        {
            glm::quat q(glm::vec3(0, mRingAnimPercent*M_PI, 0));
            mInnerRingHandle->setOrientation(q);
        }
    }

}

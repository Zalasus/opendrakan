/*
 * CrystalRingButton.cpp
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#include <dragonRfl/gui/CrystalRingButton.h>

#include <glm/gtc/matrix_transform.hpp>

#include <odCore/Exception.h>
#include <odCore/Engine.h>

#include <odCore/render/GuiNode.h>
#include <odCore/render/ModelNode.h>

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

    CrystalRingButton::CrystalRingButton(DragonGui &gui, odDb::Model *crystalModel, odDb::Model *innerRingModel, odDb::Model *outerRingModel,
            odDb::Sound *hoverSound, float noteOffset)
    : odGui::Widget(gui)
    , mCrystalColorInactive(0.38431, 0.36471, 0.54902, 1.0)
    , mCrystalColorActive(0.95686275, 0.25882353, 0.63137255, 1.0)
    , mCallbackUserData(-1)
    , mCrystalSpeedPercent(0.0)
    , mClicked(false)
    , mRingAnimPercent(0.0)
    {
        // select whatever model is not null for bounds calculation, starting with outer ring
        od::RefPtr<odDb::Model> modelForBounds =
                (crystalModel != nullptr) ? crystalModel : ((innerRingModel != nullptr) ? innerRingModel : outerRingModel);

        if(modelForBounds == nullptr)
        {
            throw od::Exception("Passed no non-null models to CrystalRingButton");
        }

        if(!modelForBounds->hasBounds())
        {
            throw od::Exception("Outermost model passed to CrystalRingButton has no bounds");
        }

        od::BoundingSphere bs = modelForBounds->getModelBounds()->getMainSphere();
        float diameter = 2 * bs.radius();

        od::OrientedBoundingBox obb = modelForBounds->getModelBounds()->getMainBox();
        glm::vec3 extends = obb.extends();
        float aspectRatio = extends.x/extends.y;

        float fovDegrees = 70;
        getRenderNode()->setPerspectiveMode(fovDegrees * M_PI/180, aspectRatio);

        if(crystalModel != nullptr)
        {
            od::RefPtr<odRender::ModelNode> mn = crystalModel->getOrCreateRenderNode(&getGui().getRenderer());

            mCrystalNode = getRenderNode()->createObjectNode();
            mCrystalNode->setModel(mn);

            mCrystalNode->setScale(glm::vec3(0.58/diameter));
            mCrystalNode->setPosition(glm::vec3(0.5, 0.5, 0));
            mCrystalNode->setGlobalLight(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 0.0, 0.0));
        }


        if(innerRingModel != nullptr)
        {
            od::RefPtr<odRender::ModelNode> mn = innerRingModel->getOrCreateRenderNode(&getGui().getRenderer());

            mInnerRingNode = getRenderNode()->createObjectNode();
            mInnerRingNode->setModel(mn);

            mInnerRingNode->setScale(glm::vec3(1/diameter));
            mInnerRingNode->setPosition(glm::vec3(0.5, 0.5, 0));
            mInnerRingNode->setGlobalLight(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 0.0, 0.0));
        }

        if(innerRingModel != nullptr)
        {
            od::RefPtr<odRender::ModelNode> mn = outerRingModel->getOrCreateRenderNode(&getGui().getRenderer());

            mOuterRingNode = getRenderNode()->createObjectNode();
            mOuterRingNode->setModel(mn);

            mOuterRingNode->setScale(glm::vec3(1/diameter));
            mOuterRingNode->setPosition(glm::vec3(0.5, 0.5, 0));
            mOuterRingNode->setGlobalLight(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 0.0, 0.0));
        }

        /*if(hoverSound != nullptr)
        {
            mSoundSource = gm.getEngine().getSoundManager().createSource();
            mSoundSource->setPosition(0.0, 0.0, 0.0);
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
        }*/
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
        /*if(mSoundSource != nullptr)
        {
            mSoundSource->play(1.0f);
        }

        mCrystalColor.move(mCrystalColorActive, 0.5);*/
    }

    void CrystalRingButton::onMouseLeave(const glm::vec2 &pos)
    {
        /*if(mSoundSource != nullptr)
        {
            mSoundSource->stop(1.8f);
        }

        mCrystalColor.move(mCrystalColorInactive, 1.0);*/
    }

    void CrystalRingButton::onUpdate(float relTime)
    {
        if(isMouseOver())
        {
            if(mCrystalSpeedPercent < 1.0)
            {
                mCrystalSpeedPercent = std::min(mCrystalSpeedPercent + relTime/OD_CRYSTAL_SPEED_RISETIME, 1.0);
            }

        }else
        {
            if(mCrystalSpeedPercent > 0.0)
            {
                mCrystalSpeedPercent = std::max(mCrystalSpeedPercent - relTime/OD_CRYSTAL_SPEED_FALLTIME, 0.0);
            }
        }

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

        if(mCrystalNode != nullptr)
        {
            // apply scaling function to speed percentage
            float crystalSpeed = isMouseOver() ?
                                    std::sin(mCrystalSpeedPercent * M_PI/2) * OD_CRYSTAL_SPEED_MAX
                                  : (std::sin((mCrystalSpeedPercent - 1)*M_PI/2)+1) * OD_CRYSTAL_SPEED_MAX;

            // apply rotation to crystal
            glm::quat q = mCrystalNode->getOrientation();
            q *= glm::quat(glm::vec3(0, -crystalSpeed*relTime, 0));
            mCrystalNode->setOrientation(q);

            _updateCrystalColor(relTime);
        }

        if(mOuterRingNode != nullptr)
        {
            glm::quat q(glm::vec3(-mRingAnimPercent*M_PI, 0, 0));
            mOuterRingNode->setOrientation(q);
        }

        if(mInnerRingNode != nullptr)
        {
            glm::quat q(glm::vec3(0, -mRingAnimPercent*M_PI, 0));
            mInnerRingNode->setOrientation(q);
        }
    }

    void CrystalRingButton::_updateCrystalColor(double relTime)
    {
        /*if(mColorModifierUniform == nullptr)
        {
            return;
        }

        if(mCrystalColor.update(relTime))
        {
            mColorModifierUniform->set(mCrystalColor);
        }*/
    }

}

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
    , mCrystalModel(crystalModel)
    , mInnerRingModel(innerRingModel)
    , mOuterRingModel(outerRingModel)
    , mCrystalColorInactive(0.38431, 0.36471, 0.54902, 1.0)
    , mCrystalColorActive(0.95686275, 0.25882353, 0.63137255, 1.0)
    , mCallbackUserData(-1)
    , mCrystalSpeedPercent(0.0)
    , mClicked(false)
    , mRingAnimPercent(0.0)
    {
        // select whatever model is not null for bounds calculation, starting with outer ring
        od::RefPtr<odDb::Model> modelForBounds =
                (mOuterRingModel != nullptr) ? mOuterRingModel : ((mInnerRingModel != nullptr) ? mInnerRingModel : mCrystalModel);

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

        glm::mat4 matrix(1.0);
        matrix = glm::translate(matrix, -bs.center());
        matrix = glm::scale(matrix, glm::vec3(1/diameter));
        matrix = glm::translate(matrix, glm::vec3(0.5, 0.5, 0.0));
        mTransform->setMatrix(matrix);

        mTransform->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
        mTransform->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        if(mCrystalModel != nullptr)
        {
            od::RefPtr<odRender::ModelNode> mn = mCrystalModel->getOrCreateRenderNode(&mGui.getRenderer());

            mCrystalTransform = new osg::PositionAttitudeTransform;
            mCrystalTransform->addChild(mCrystalModel->getOrBuildNode(gm.getEngine().getRenderManager()));

            osg::StateSet *ss = mCrystalTransform->getOrCreateStateSet();
            ss->setDefine("COLOR_MODIFIER");

            mColorModifierUniform = new osg::Uniform("colorModifier", mCrystalColorInactive);
            ss->addUniform(mColorModifierUniform);

            mCrystalTransform->setScale(osg::Vec3(0.58, 0.58, 0.58));
            mCrystalTransform->setAttitude(osg::Quat(M_PI, osg::Vec3(0, 1, 0)));

            mTransform->addChild(mCrystalTransform);
        }

        if(mInnerRingModel != nullptr)
        {
            mInnerRingTransform = new osg::PositionAttitudeTransform;
            mInnerRingTransform->addChild(mInnerRingModel->getOrBuildNode(gm.getEngine().getRenderManager()));
            mTransform->addChild(mInnerRingTransform);
        }

        if(mOuterRingModel != nullptr)
        {
            mOuterRingTransform = new osg::PositionAttitudeTransform;
            mOuterRingTransform->addChild(mOuterRingModel->getOrBuildNode(gm.getEngine().getRenderManager()));
            mTransform->addChild(mOuterRingTransform);
        }

        this->addChild(mTransform);

        if(hoverSound != nullptr)
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
        }
    }

    bool CrystalRingButton::liesWithinLogicalArea(const osg::Vec2 &pos)
    {
        // logical area is circle around (0.5 0.5) TODO: implement non-circular buttons here
        osg::Vec2 p = pos - osg::Vec2(0.5, 0.5);
        return p.length2() <= 0.25;
    }

    void CrystalRingButton::onMouseDown(const osg::Vec2 &pos, int button)
    {
        mClicked = true;
    }

    void CrystalRingButton::onMouseEnter(const osg::Vec2 &pos)
    {
        if(mSoundSource != nullptr)
        {
            mSoundSource->play(1.0f);
        }

        mCrystalColor.move(mCrystalColorActive, 0.5);
    }

    void CrystalRingButton::onMouseLeave(const osg::Vec2 &pos)
    {
        if(mSoundSource != nullptr)
        {
            mSoundSource->stop(1.8f);
        }

        mCrystalColor.move(mCrystalColorInactive, 1.0);
    }

    void CrystalRingButton::onUpdate(double simTime, double relTime)
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

        if(mCrystalTransform != nullptr)
        {
            // apply scaling function to speed percentage
            float crystalSpeed = isMouseOver() ?
                                    std::sin(mCrystalSpeedPercent * M_PI/2) * OD_CRYSTAL_SPEED_MAX
                                  : (std::sin((mCrystalSpeedPercent - 1)*M_PI/2)+1) * OD_CRYSTAL_SPEED_MAX;

            // apply rotation to crystal
            osg::Quat q = mCrystalTransform->getAttitude();
            q *= osg::Quat(crystalSpeed * relTime, osg::Vec3(0, -1, 0));
            mCrystalTransform->setAttitude(q);

            _updateCrystalColor(relTime);
        }

        if(mOuterRingTransform != nullptr)
        {
            osg::Quat q(mRingAnimPercent * M_PI, osg::Vec3(1, 0, 0));
            mOuterRingTransform->setAttitude(q);
        }

        if(mInnerRingTransform != nullptr)
        {
            osg::Quat q(mRingAnimPercent * M_PI, osg::Vec3(0, 1, 0));
            mInnerRingTransform->setAttitude(q);
        }
    }

    void CrystalRingButton::_updateCrystalColor(double relTime)
    {
        if(mColorModifierUniform == nullptr)
        {
            return;
        }

        if(mCrystalColor.update(relTime))
        {
            mColorModifierUniform->set(mCrystalColor);
        }
    }

}

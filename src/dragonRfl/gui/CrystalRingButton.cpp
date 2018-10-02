/*
 * CrystalRingButton.cpp
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#include <dragonRfl/gui/CrystalRingButton.h>

#include <odCore/Exception.h>
#include <odCore/Engine.h>
#include <odCore/gui/GuiManager.h>
#include <odCore/ShaderManager.h>
#include <odCore/audio/SoundManager.h>

// max crystal speed in rad/s
#define OD_CRYSTAL_SPEED_MAX       (3.0*M_PI)

// time a crystal needs to speed up from stop to max speed
#define OD_CRYSTAL_SPEED_RISETIME   3.0

// time a crystal at max speed needs to decelerate to stop
#define OD_CRYSTAL_SPEED_FALLTIME   3.0

// time a ring need to perform a full rotation
#define OD_RING_SPEED 1.0

namespace od
{

    CrystalRingButton::CrystalRingButton(GuiManager &gm, Model *crystalModel, Model *innerRingModel, Model *outerRingModel,
            Sound *hoverSound, float noteOffset)
    : Widget(gm)
    , mSoundSource(nullptr)
    , mCrystalModel(crystalModel)
    , mInnerRingModel(innerRingModel)
    , mOuterRingModel(outerRingModel)
    , mCrystalColorInactive(0.38431, 0.36471, 0.54902, 1.0)
    , mCrystalColorActive(0.95686275, 0.25882353, 0.63137255, 1.0)
    , mCrystalColor(mCrystalColorInactive)
    , mTransform(new osg::MatrixTransform)
    , mCallbackUserData(-1)
    , mCrystalSpeedPercent(0.0)
    , mClicked(false)
    , mRingAnimPercent(0.0)
    {
        // select whatever model is not null for bounds calculation, starting with outer ring
        osg::ref_ptr<Model> modelForBounds =
                (mOuterRingModel != nullptr) ? mOuterRingModel : ((mInnerRingModel != nullptr) ? mInnerRingModel : mCrystalModel);

        if(modelForBounds == nullptr)
        {
            throw Exception("Passed no non-null models to CrystalRingButton");
        }

        osg::BoundingBox box = modelForBounds->getCalculatedBoundingBox();
        float diameter = box.yMax() - box.yMin(); // ring's "hole" is parallel to y axis, so this should be the best way to get the diameter

        osg::Matrix matrix = osg::Matrix::identity();
        matrix.postMultTranslate(-box.center());
        matrix.postMultScale(osg::Vec3(1/diameter, 1/diameter, 1/diameter));
        matrix.postMultTranslate(osg::Vec3(0.5, 0.5, 0.0));
        mTransform->setMatrix(matrix);

        mTransform->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
        mTransform->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        if(mCrystalModel != nullptr)
        {
            mCrystalTransform = new osg::PositionAttitudeTransform;
            mCrystalTransform->addChild(mCrystalModel);

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
            mInnerRingTransform->addChild(mInnerRingModel);
            mTransform->addChild(mInnerRingTransform);
        }

        if(mOuterRingModel != nullptr)
        {
            mOuterRingTransform = new osg::PositionAttitudeTransform;
            mOuterRingTransform->addChild(mOuterRingModel);
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

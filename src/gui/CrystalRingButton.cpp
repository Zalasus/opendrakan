/*
 * CrystalRingButton.cpp
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#include "gui/CrystalRingButton.h"

#include "Exception.h"
#include "gui/GuiManager.h"
#include "Engine.h"

namespace od
{

    CrystalRingButton::CrystalRingButton(GuiManager &gm, Model *crystalModel, Model *innerRingModel, Model *outerRingModel)
    : Widget(gm)
    , mCrystalModel(crystalModel)
    , mInnerRingModel(innerRingModel)
    , mOuterRingModel(outerRingModel)
    , mCrystalColor(0.38431, 0.36471, 0.54902, 1.0)
    , mTransform(new osg::MatrixTransform)
    {
        // select whatever model in not null for bounds calculation, starting with outer ring
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

            osg::ref_ptr<osg::Shader> crystalFragShader = gm.getEngine().getShaderManager().loadShader("crystal_fragment.glsl", osg::Shader::FRAGMENT);
            osg::ref_ptr<osg::Program> crystalProg = gm.getEngine().getShaderManager().makeProgram(nullptr, crystalFragShader);
            mCrystalTransform->getOrCreateStateSet()->setAttribute(crystalProg, osg::StateAttribute::ON);

            mColorModifierUniform = new osg::Uniform("colorModifier", mCrystalColor);
            mCrystalTransform->getOrCreateStateSet()->addUniform(mColorModifierUniform);

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
    }

    void CrystalRingButton::onMouseDown(osg::Vec2 pos, int button)
    {
        Logger::info() << "Button clicked!!!";
    }

}

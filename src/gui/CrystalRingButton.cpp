/*
 * CrystalRingButton.cpp
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#include "gui/CrystalRingButton.h"

#include "Exception.h"

namespace od
{

    CrystalRingButton::CrystalRingButton(GuiManager &gm, Model *crystalModel, Model *innerRingModel, Model *outerRingModel)
    : Widget(gm)
    , mCrystalModel(crystalModel)
    , mInnerRingModel(innerRingModel)
    , mOuterRingModel(outerRingModel)
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

        if(mCrystalModel != nullptr)
        {
            mTransform->addChild(mCrystalModel);
        }

        if(mInnerRingModel != nullptr)
        {
            mTransform->addChild(mInnerRingModel);
        }

        if(mOuterRingModel != nullptr)
        {
            mTransform->addChild(mOuterRingModel);
        }

        this->addChild(mTransform);
    }

}

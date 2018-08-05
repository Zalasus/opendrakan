/*
 * CrystalRingButton.h
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CRYSTALRINGBUTTON_H_
#define INCLUDE_GUI_CRYSTALRINGBUTTON_H_

#include "gui/Widget.h"
#include "db/Model.h"

#include <osg/MatrixTransform>

namespace od
{

    class CrystalRingButton : public Widget
    {
    public:

        CrystalRingButton(GuiManager &gm, Model *crystalModel, Model *innerRingModel, Model *outerRingModel);

        inline void setCrystalColor(const osg::Vec4 &color) { mCrystalColor = color; mColorModifierUniform->set(mCrystalColor); }


    private:

        osg::ref_ptr<Model> mCrystalModel;
        osg::ref_ptr<Model> mInnerRingModel;
        osg::ref_ptr<Model> mOuterRingModel;
        osg::Vec4 mCrystalColor;
        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::Uniform> mColorModifierUniform;

    };

}

#endif /* INCLUDE_GUI_CRYSTALRINGBUTTON_H_ */

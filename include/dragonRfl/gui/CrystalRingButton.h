/*
 * CrystalRingButton.h
 *
 *  Created on: 5 Aug 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CRYSTALRINGBUTTON_H_
#define INCLUDE_GUI_CRYSTALRINGBUTTON_H_

#include <functional>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <odCore/gui/Widget.h>
#include <odCore/db/Model.h>
#include <odCore/db/Sound.h>
#include <odCore/audio/Source.h>

namespace od
{

    class CrystalRingButton : public Widget
    {
    public:

        CrystalRingButton(GuiManager &gm, Model *crystalModel, Model *innerRingModel, Model *outerRingModel, Sound *hoverSound, float soundPitch);

        inline void setInactiveCrystalColor(const osg::Vec4 &color) { mCrystalColorInactive = color; _updateCrystalColor(); }
        inline void setActiveCrystalColor(const osg::Vec4 &color) { mCrystalColorActive = color;  _updateCrystalColor(); }
        inline void setClickedCallback(const std::function<void(int)> &callback, int userData) { mClickedCallback = callback; mCallbackUserData = userData; }

        virtual bool liesWithinLogicalArea(const osg::Vec2 &pos) override;
        virtual void onMouseDown(const osg::Vec2 &pos, int button) override;
        virtual void onMouseEnter(const osg::Vec2 &pos) override;
        virtual void onMouseLeave(const osg::Vec2 &pos) override;
        virtual void onUpdate(double simTime, double relTime) override;


    private:

        void _updateCrystalColor();

        Source *mSoundSource;
        osg::ref_ptr<Model> mCrystalModel;
        osg::ref_ptr<Model> mInnerRingModel;
        osg::ref_ptr<Model> mOuterRingModel;
        osg::Vec4 mCrystalColorInactive;
        osg::Vec4 mCrystalColorActive;
        osg::ref_ptr<osg::MatrixTransform> mTransform;
        osg::ref_ptr<osg::PositionAttitudeTransform> mCrystalTransform;
        osg::ref_ptr<osg::PositionAttitudeTransform> mOuterRingTransform;
        osg::ref_ptr<osg::PositionAttitudeTransform> mInnerRingTransform;
        osg::ref_ptr<osg::Uniform> mColorModifierUniform;
        std::function<void(int)> mClickedCallback;
        int mCallbackUserData;

        float mCrystalSpeedPercent; // 0=stop, 1=max speed
        bool mClicked;
        float mRingAnimPercent;

    };

}

#endif /* INCLUDE_GUI_CRYSTALRINGBUTTON_H_ */

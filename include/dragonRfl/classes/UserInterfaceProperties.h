/*
 * UserInterfaceProperties.h
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_
#define INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_

#include <odCore/rfl/Class.h>
#include <odCore/rfl/Field.h>
#include <odCore/rfl/AssetRefField.h>

namespace dragonRfl
{

    class UserInterfaceProperties : public odRfl::ClassBase
    {
    public:

        UserInterfaceProperties();

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    //protected:

        odRfl::ModelRef     mInnerRing;
        odRfl::ModelRef     mOuterRing;
        odRfl::ModelRef     mCrystalTop;
        odRfl::Float        mCrystalTopNoteOffset;
        odRfl::ModelRef     mCrystalLeft;
        odRfl::Float        mCrystalLeftNoteOffset;
        odRfl::ModelRef     mCrystalMiddle;
        odRfl::Float        mCrystalMiddleNoteOffset;
        odRfl::ModelRef     mCrystalRight;
        odRfl::Float        mCrystalRightNoteOffset;
        odRfl::ModelRef     mCrystalLowerLeft;
        odRfl::Float        mCrystalLowerLeftNoteOffset;
        odRfl::ModelRef     mCrystalLowerRight;
        odRfl::Float        mCrystalLowerRightNoteOffset;
        odRfl::ModelRef     mCrystalBottom;
        odRfl::Float        mCrystalBottomNoteOffset;
        odRfl::ModelRef     mArrowUpCrystal;
        odRfl::ModelRef     mButtonCrystal;
        odRfl::ModelRef     mWideButtonCrystal;
        odRfl::ModelRef     mNormalButtonRing;
        odRfl::ModelRef     mWideButtonRing;
        odRfl::SoundRef     mHoverSoundLooped;
        odRfl::SoundRef     mButtonClickSound;
        odRfl::SoundRef     mMainGuiButtonClickSound;
        odRfl::SoundRef     mScrollbarClickSound;
        odRfl::SoundRef     mScrollbarReleaseSound;
        odRfl::SoundRef     mScrollbarSlideLoopingSound;
        odRfl::SoundRef     mNextOptionSound;
        odRfl::SoundRef     mPreviousOptionSound;
        odRfl::SoundRef     mMessageBoxSound;
        odRfl::SoundRef     mExitSound;
        odRfl::Integer      mBackgroundMusicId;
        odRfl::Integer      mCreditsMusicId;
        odRfl::Integer      mEasyScalePercent;
        odRfl::Integer      mHardScalePercent;
        odRfl::String       mIntroLevelFilename;
        odRfl::String       mFirstLevelFilename;
    };

}

ODRFL_DEFINE_CLASS_BASE(dragonRfl::UserInterfaceProperties, 0x0062, "System", "User Interface Properties");

#endif /* INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_ */

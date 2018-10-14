/*
 * UserInterfaceProperties.h
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#ifndef INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_
#define INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_

#include <odCore/rfl/RflClass.h>
#include <odCore/rfl/RflField.h>

namespace dragonRfl
{

    class DragonRfl;

    class UserInterfaceProperties : public odRfl::RflClass
    {
    public:

        UserInterfaceProperties(DragonRfl &rfl);

        virtual void probeFields(odRfl::FieldProbe &probe) override;


    //protected:

        odRfl::RflModelRef     mInnerRing;
        odRfl::RflModelRef     mOuterRing;
        odRfl::RflModelRef     mCrystalTop;
        odRfl::RflFloat        mCrystalTopNoteOffset;
        odRfl::RflModelRef     mCrystalLeft;
        odRfl::RflFloat        mCrystalLeftNoteOffset;
        odRfl::RflModelRef     mCrystalMiddle;
        odRfl::RflFloat        mCrystalMiddleNoteOffset;
        odRfl::RflModelRef     mCrystalRight;
        odRfl::RflFloat        mCrystalRightNoteOffset;
        odRfl::RflModelRef     mCrystalLowerLeft;
        odRfl::RflFloat        mCrystalLowerLeftNoteOffset;
        odRfl::RflModelRef     mCrystalLowerRight;
        odRfl::RflFloat        mCrystalLowerRightNoteOffset;
        odRfl::RflModelRef     mCrystalBottom;
        odRfl::RflFloat        mCrystalBottomNoteOffset;
        odRfl::RflModelRef     mArrowUpCrystal;
        odRfl::RflModelRef     mButtonCrystal;
        odRfl::RflModelRef     mWideButtonCrystal;
        odRfl::RflModelRef     mNormalButtonRing;
        odRfl::RflModelRef     mWideButtonRing;
        odRfl::RflSoundRef     mHoverSoundLooped;
        odRfl::RflSoundRef     mButtonClickSound;
        odRfl::RflSoundRef     mMainGuiButtonClickSound;
        odRfl::RflSoundRef     mScrollbarClickSound;
        odRfl::RflSoundRef     mScrollbarReleaseSound;
        odRfl::RflSoundRef     mScrollbarSlideLoopingSound;
        odRfl::RflSoundRef     mNextOptionSound;
        odRfl::RflSoundRef     mPreviousOptionSound;
        odRfl::RflSoundRef     mMessageBoxSound;
        odRfl::RflSoundRef     mExitSound;
        odRfl::RflInteger      mBackgroundMusicId;
        odRfl::RflInteger      mCreditsMusicId;
        odRfl::RflInteger      mEasyScalePercent;
        odRfl::RflInteger      mHardScalePercent;
        odRfl::RflString       mIntroLevelFilename;
        odRfl::RflString       mFirstLevelFilename;
    };

}

OD_DEFINE_RFLCLASS_TRAITS(dragonRfl::DragonRfl, 0x0062, "System", "User Interface Properties", dragonRfl::UserInterfaceProperties);

#endif /* INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_ */

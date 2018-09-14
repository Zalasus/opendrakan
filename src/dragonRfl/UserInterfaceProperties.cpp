/*
 * UserInterfaceProperties.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include <dragonRfl/UserInterfaceProperties.h>

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

    UserInterfaceProperties::UserInterfaceProperties()
    : mInnerRing(od::AssetRef::NULL_REF)
    , mOuterRing(od::AssetRef::NULL_REF)
    , mCrystalTop(od::AssetRef::NULL_REF)
    , mCrystalTopNoteOffset(0.0)
    , mCrystalLeft(od::AssetRef::NULL_REF)
    , mCrystalLeftNoteOffset(0.0)
    , mCrystalMiddle(od::AssetRef::NULL_REF)
    , mCrystalMiddleNoteOffset(0.0)
    , mCrystalRight(od::AssetRef::NULL_REF)
    , mCrystalRightNoteOffset(0.0)
    , mCrystalLowerLeft(od::AssetRef::NULL_REF)
    , mCrystalLowerLeftNoteOffset(0.0)
    , mCrystalLowerRight(od::AssetRef::NULL_REF)
    , mCrystalLowerRightNoteOffset(0.0)
    , mCrystalBottom(od::AssetRef::NULL_REF)
    , mCrystalBottomNoteOffset(0.0)
    , mArrowUpCrystal(od::AssetRef::NULL_REF)
    , mButtonCrystal(od::AssetRef::NULL_REF)
    , mWideButtonCrystal(od::AssetRef::NULL_REF)
    , mNormalButtonRing(od::AssetRef::NULL_REF)
    , mWideButtonRing(od::AssetRef::NULL_REF)
    , mHoverSoundLooped(od::AssetRef::NULL_REF)
    , mButtonClickSound(od::AssetRef::NULL_REF)
    , mMainGuiButtonClickSound(od::AssetRef::NULL_REF)
    , mScrollbarClickSound(od::AssetRef::NULL_REF)
    , mScrollbarReleaseSound(od::AssetRef::NULL_REF)
    , mScrollbarSlideLoopingSound(od::AssetRef::NULL_REF)
    , mNextOptionSound(od::AssetRef::NULL_REF)
    , mPreviousOptionSound(od::AssetRef::NULL_REF)
    , mMessageBoxSound(od::AssetRef::NULL_REF)
    , mExitSound(od::AssetRef::NULL_REF)
    , mBackgroundMusicId(0)
    , mCreditsMusicId(0)
    , mEasyScalePercent(75)
    , mHardScalePercent(128)
    , mIntroLevelFilename("")
    , mFirstLevelFilename("")
    {
    }

    void UserInterfaceProperties::probeFields(RflFieldProbe &probe)
    {
        probe("Main Menu")
                  (mInnerRing, "Inner Ring")
                  (mOuterRing, "Outer Ring")
                  (mCrystalTop, "Crystal Top")
                  (mCrystalTopNoteOffset, "Crystal Top Note Offset")
                  (mCrystalLeft, "Crystal Left")
                  (mCrystalLeftNoteOffset, "Crystal Left Note Offset")
                  (mCrystalMiddle, "Crystal Middle")
                  (mCrystalMiddleNoteOffset, "Crystal Middle Note Offset")
                  (mCrystalRight, "Crystal Right")
                  (mCrystalRightNoteOffset, "Crystal Right Note Offset")
                  (mCrystalLowerLeft, "Crystal Lower Left")
                  (mCrystalLowerLeftNoteOffset, "Crystal Lower Left Note Offset")
                  (mCrystalLowerRight, "Crystal Lower Right")
                  (mCrystalLowerRightNoteOffset, "Crystal Lower Right Note Offset")
                  (mCrystalBottom, "Crystal Bottom")
                  (mCrystalBottomNoteOffset, "Crystal Bottom Note Offset")
              ("Common Buttons")
                  (mArrowUpCrystal, "Arrow Up Crystal")
                  (mButtonCrystal, "Button Crystal")
                  (mWideButtonCrystal, "Wide Button Crystal")
                  (mNormalButtonRing, "Normal Button Ring")
                  (mWideButtonRing, "Wide Button Ring")
              ("Sounds And Music")
                  (mHoverSoundLooped, "Hover Sound (LOOPED)")
                  (mButtonClickSound, "Button Click Sound")
                  (mMainGuiButtonClickSound, "Main Gui Button Click Sound")
                  (mScrollbarClickSound, "Scrollbar Click Sound")
                  (mScrollbarReleaseSound, "Scrollbar Release Sound")
                  (mScrollbarSlideLoopingSound, "Scrollbar Slide Looping Sound")
                  (mNextOptionSound, "Next Option")
                  (mPreviousOptionSound, "Previous Option")
                  (mMessageBoxSound, "Message Box Sound")
                  (mExitSound, "Exit Sound")
                  (mBackgroundMusicId, "Background Music ID")
                  (mCreditsMusicId, "Credits Music ID")
              ("Difficulty Health Scaling")
                  (mEasyScalePercent, "Easy Scale (%)")
                  (mHardScalePercent, "Hard Scale (%)")
              ("Level Files")
                  (mIntroLevelFilename, "Intro Level Filename")
                  (mFirstLevelFilename, "First Level Filename");

    }


    OD_REGISTER_RFL_CLASS(0x0062, "User Interface Properties", UserInterfaceProperties);
}


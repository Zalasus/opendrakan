/*
 * UserInterfaceProperties.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: zal
 */

#include <dragonRfl/classes/UserInterfaceProperties.h>

#include <dragonRfl/RflDragon.h>

namespace dragonRfl
{

    UserInterfaceProperties::UserInterfaceProperties()
    : mInnerRing(odDb::AssetRef::NULL_REF)
    , mOuterRing(odDb::AssetRef::NULL_REF)
    , mCrystalTop(odDb::AssetRef::NULL_REF)
    , mCrystalTopNoteOffset(0.0)
    , mCrystalLeft(odDb::AssetRef::NULL_REF)
    , mCrystalLeftNoteOffset(0.0)
    , mCrystalMiddle(odDb::AssetRef::NULL_REF)
    , mCrystalMiddleNoteOffset(0.0)
    , mCrystalRight(odDb::AssetRef::NULL_REF)
    , mCrystalRightNoteOffset(0.0)
    , mCrystalLowerLeft(odDb::AssetRef::NULL_REF)
    , mCrystalLowerLeftNoteOffset(0.0)
    , mCrystalLowerRight(odDb::AssetRef::NULL_REF)
    , mCrystalLowerRightNoteOffset(0.0)
    , mCrystalBottom(odDb::AssetRef::NULL_REF)
    , mCrystalBottomNoteOffset(0.0)
    , mArrowUpCrystal(odDb::AssetRef::NULL_REF)
    , mButtonCrystal(odDb::AssetRef::NULL_REF)
    , mWideButtonCrystal(odDb::AssetRef::NULL_REF)
    , mNormalButtonRing(odDb::AssetRef::NULL_REF)
    , mWideButtonRing(odDb::AssetRef::NULL_REF)
    , mHoverSoundLooped(odDb::AssetRef::NULL_REF)
    , mButtonClickSound(odDb::AssetRef::NULL_REF)
    , mMainGuiButtonClickSound(odDb::AssetRef::NULL_REF)
    , mScrollbarClickSound(odDb::AssetRef::NULL_REF)
    , mScrollbarReleaseSound(odDb::AssetRef::NULL_REF)
    , mScrollbarSlideLoopingSound(odDb::AssetRef::NULL_REF)
    , mNextOptionSound(odDb::AssetRef::NULL_REF)
    , mPreviousOptionSound(odDb::AssetRef::NULL_REF)
    , mMessageBoxSound(odDb::AssetRef::NULL_REF)
    , mExitSound(odDb::AssetRef::NULL_REF)
    , mBackgroundMusicId(0)
    , mCreditsMusicId(0)
    , mEasyScalePercent(75)
    , mHardScalePercent(128)
    , mIntroLevelFilename("")
    , mFirstLevelFilename("")
    {
    }

    void UserInterfaceProperties::probeFields(odRfl::FieldProbe &probe)
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

}


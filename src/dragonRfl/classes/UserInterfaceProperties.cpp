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

    UserInterfacePropertiesFields::UserInterfacePropertiesFields()
    : innerRing(odDb::AssetRef::NULL_REF)
    , outerRing(odDb::AssetRef::NULL_REF)
    , crystalTop(odDb::AssetRef::NULL_REF)
    , crystalTopNoteOffset(0.0)
    , crystalLeft(odDb::AssetRef::NULL_REF)
    , crystalLeftNoteOffset(0.0)
    , crystalMiddle(odDb::AssetRef::NULL_REF)
    , crystalMiddleNoteOffset(0.0)
    , crystalRight(odDb::AssetRef::NULL_REF)
    , crystalRightNoteOffset(0.0)
    , crystalLowerLeft(odDb::AssetRef::NULL_REF)
    , crystalLowerLeftNoteOffset(0.0)
    , crystalLowerRight(odDb::AssetRef::NULL_REF)
    , crystalLowerRightNoteOffset(0.0)
    , crystalBottom(odDb::AssetRef::NULL_REF)
    , crystalBottomNoteOffset(0.0)
    , arrowUpCrystal(odDb::AssetRef::NULL_REF)
    , buttonCrystal(odDb::AssetRef::NULL_REF)
    , wideButtonCrystal(odDb::AssetRef::NULL_REF)
    , normalButtonRing(odDb::AssetRef::NULL_REF)
    , wideButtonRing(odDb::AssetRef::NULL_REF)
    , hoverSoundLooped(odDb::AssetRef::NULL_REF)
    , buttonClickSound(odDb::AssetRef::NULL_REF)
    , mainGuiButtonClickSound(odDb::AssetRef::NULL_REF)
    , scrollbarClickSound(odDb::AssetRef::NULL_REF)
    , scrollbarReleaseSound(odDb::AssetRef::NULL_REF)
    , scrollbarSlideLoopingSound(odDb::AssetRef::NULL_REF)
    , nextOptionSound(odDb::AssetRef::NULL_REF)
    , previousOptionSound(odDb::AssetRef::NULL_REF)
    , messageBoxSound(odDb::AssetRef::NULL_REF)
    , exitSound(odDb::AssetRef::NULL_REF)
    , backgroundMusicId(0)
    , creditsMusicId(0)
    , easyScalePercent(75)
    , hardScalePercent(128)
    , introLevelFilename("")
    , firstLevelFilename("")
    {
    }

    void UserInterfacePropertiesFields::probeFields(odRfl::FieldProbe &probe)
    {
        probe("Main Menu")
                  (innerRing, "Inner Ring")
                  (outerRing, "Outer Ring")
                  (crystalTop, "Crystal Top")
                  (crystalTopNoteOffset, "Crystal Top Note Offset")
                  (crystalLeft, "Crystal Left")
                  (crystalLeftNoteOffset, "Crystal Left Note Offset")
                  (crystalMiddle, "Crystal Middle")
                  (crystalMiddleNoteOffset, "Crystal Middle Note Offset")
                  (crystalRight, "Crystal Right")
                  (crystalRightNoteOffset, "Crystal Right Note Offset")
                  (crystalLowerLeft, "Crystal Lower Left")
                  (crystalLowerLeftNoteOffset, "Crystal Lower Left Note Offset")
                  (crystalLowerRight, "Crystal Lower Right")
                  (crystalLowerRightNoteOffset, "Crystal Lower Right Note Offset")
                  (crystalBottom, "Crystal Bottom")
                  (crystalBottomNoteOffset, "Crystal Bottom Note Offset")
              ("Common Buttons")
                  (arrowUpCrystal, "Arrow Up Crystal")
                  (buttonCrystal, "Button Crystal")
                  (wideButtonCrystal, "Wide Button Crystal")
                  (normalButtonRing, "Normal Button Ring")
                  (wideButtonRing, "Wide Button Ring")
              ("Sounds And Music")
                  (hoverSoundLooped, "Hover Sound (LOOPED)")
                  (buttonClickSound, "Button Click Sound")
                  (mainGuiButtonClickSound, "Main Gui Button Click Sound")
                  (scrollbarClickSound, "Scrollbar Click Sound")
                  (scrollbarReleaseSound, "Scrollbar Release Sound")
                  (scrollbarSlideLoopingSound, "Scrollbar Slide Looping Sound")
                  (nextOptionSound, "Next Option")
                  (previousOptionSound, "Previous Option")
                  (messageBoxSound, "Message Box Sound")
                  (exitSound, "Exit Sound")
                  (backgroundMusicId, "Background Music ID")
                  (creditsMusicId, "Credits Music ID")
              ("Difficulty Health Scaling")
                  (easyScalePercent, "Easy Scale (%)")
                  (hardScalePercent, "Hard Scale (%)")
              ("Level Files")
                  (introLevelFilename, "Intro Level Filename")
                  (firstLevelFilename, "First Level Filename");

    }

}

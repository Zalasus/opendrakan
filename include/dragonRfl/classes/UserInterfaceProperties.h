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

    struct UserInterfacePropertiesFields final : public odRfl::FieldBundle
    {
        UserInterfacePropertiesFields();

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        odRfl::ModelRef     innerRing;
        odRfl::ModelRef     outerRing;
        odRfl::ModelRef     crystalTop;
        odRfl::Float        crystalTopNoteOffset;
        odRfl::ModelRef     crystalLeft;
        odRfl::Float        crystalLeftNoteOffset;
        odRfl::ModelRef     crystalMiddle;
        odRfl::Float        crystalMiddleNoteOffset;
        odRfl::ModelRef     crystalRight;
        odRfl::Float        crystalRightNoteOffset;
        odRfl::ModelRef     crystalLowerLeft;
        odRfl::Float        crystalLowerLeftNoteOffset;
        odRfl::ModelRef     crystalLowerRight;
        odRfl::Float        crystalLowerRightNoteOffset;
        odRfl::ModelRef     crystalBottom;
        odRfl::Float        crystalBottomNoteOffset;
        odRfl::ModelRef     arrowUpCrystal;
        odRfl::ModelRef     buttonCrystal;
        odRfl::ModelRef     wideButtonCrystal;
        odRfl::ModelRef     normalButtonRing;
        odRfl::ModelRef     wideButtonRing;
        odRfl::SoundRef     hoverSoundLooped;
        odRfl::SoundRef     buttonClickSound;
        odRfl::SoundRef     mainGuiButtonClickSound;
        odRfl::SoundRef     scrollbarClickSound;
        odRfl::SoundRef     scrollbarReleaseSound;
        odRfl::SoundRef     scrollbarSlideLoopingSound;
        odRfl::SoundRef     nextOptionSound;
        odRfl::SoundRef     previousOptionSound;
        odRfl::SoundRef     messageBoxSound;
        odRfl::SoundRef     exitSound;
        odRfl::Integer      backgroundMusicId;
        odRfl::Integer      creditsMusicId;
        odRfl::Integer      easyScalePercent;
        odRfl::Integer      hardScalePercent;
        odRfl::String       introLevelFilename;
        odRfl::String       firstLevelFilename;
    };


    OD_DEFINE_CLASS(UserInterfaceProperties, 0x0062, "System", "User Interface Properties", odRfl::FieldsOnlyClassFactory<UserInterfacePropertiesFields>);

}

#endif /* INCLUDE_RFL_DRAGON_USERINTERFACEPROPERTIES_H_ */

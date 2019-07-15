/*
 * DMusicDefines.h
 *
 *  Created on: Jul 13, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_AUDIO_MUSIC_DMUSICDEFINES_H_
#define INCLUDE_ODCORE_AUDIO_MUSIC_DMUSICDEFINES_H_

namespace directmusic
{

    enum curve_type
    {
        DMUS_CURVET_PBCURVE   = 0x03, // pitch bend
        DMUS_CURVET_CCCURVE   = 0x04, // continous controller
        DMUS_CURVET_MATCURVE  = 0x05, // monophonic aftertouch
        DMUS_CURVET_PATCURVE  = 0x06, // polyphonic aftertouch
        DMUS_CURVET_RPNCURVE  = 0x07, // registered parameter number curve (probably DX8)
        DMUS_CURVET_NRPNCURVE = 0x08  // non-registered parameter number curve (probably DX8)
    };


    enum curve_shape
    {
         DMUS_CURVES_LINEAR  = 0x0,
         DMUS_CURVES_INSTANT = 0x1,
         DMUS_CURVES_EXP     = 0x2,
         DMUS_CURVES_LOG     = 0x3,
         DMUS_CURVES_SINE    = 0x4
    };


    enum instrument_flags
    {
        DMUS_IO_INST_PATCH              = 0x0001,
        DMUS_IO_INST_BANKSELECT         = 0x0002,
        DMUS_IO_INST_ASSIGN_PATCH       = 0x0008,
        DMUS_IO_INST_NOTERANGES         = 0x0010,
        DMUS_IO_INST_PAN                = 0x0020,
        DMUS_IO_INST_VOLUME             = 0x0040,
        DMUS_IO_INST_TRANSPOSE          = 0x0080,
        DMUS_IO_INST_GM                 = 0x0100,
        DMUS_IO_INST_GS                 = 0x0200,
        DMUS_IO_INST_XG                 = 0x0400,
        DMUS_IO_INST_CHANNEL_PRIORITY   = 0x0800,
        DMUS_IO_INST_USE_DEFAULT_GM_SET = 0x1000,
        DMUS_IO_INST_PITCHBENDRANGE     = 0x2000
    };
}

#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_DMUSICDEFINES_H_ */

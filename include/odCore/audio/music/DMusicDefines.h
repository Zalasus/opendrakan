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
}

#endif /* INCLUDE_ODCORE_AUDIO_MUSIC_DMUSICDEFINES_H_ */

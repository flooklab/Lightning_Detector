/*
////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of the firmware of Lightning Detector, an experimental open
//  hardware project for early notification about approaching thunderstorm activity.
//  Copyright (C) 2024–2025 M. Frohne
//
//  Lightning Detector's firmware is free software: you can redistribute it
//  and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation, either version 3
//  of the License, or (at your option) any later version.
//
//  Lightning Detector's firmware is distributed in the hope that it
//  will be useful, but WITHOUT ANY WARRANTY; without even the implied
//  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Lightning Detector. If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "usedisplay.h"

#ifndef USE_DISPLAY
    #define USE_DISPLAY USE_DISPLAY_NONE
#endif

#if USE_DISPLAY == USE_DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER
    #include "display_waveshareepaper154bw_sx1509ioexpander.h"
#elif USE_DISPLAY == USE_DISPLAY_NONE
    #include "display_dummy.h"
#endif

#if USE_DISPLAY == USE_DISPLAY_NONE
    static constexpr bool useDisplay = false;
#else
    static constexpr bool useDisplay = true;
#endif

typedef DISPLAY_TYPE Display;

#endif // DISPLAY_H

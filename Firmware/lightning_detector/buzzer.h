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

#ifndef BUZZER_H
#define BUZZER_H

#include "pins.h"

#include <Arduino.h>

/*!
 * \brief Driver class for the piezoelectric buzzer.
 *
 * Provides beep functions as well as a "buzzer response test" to assess the
 * achievable precision of the acoustic tuning routine for the AS3935 antenna.
 *
 * \attention You must call setup() before using the class.
 */
class Buzzer
{
public:
    Buzzer(Pin pPin, float pFrequency, float pDutyCycle);   ///< Constructor.
    //
    void setup() const;                                     ///< \copybrief AS3935::setup()
    //
    void beepSingle(float pSeconds) const;                                          ///< Play a single beep.
    void beepMulti(size_t pTimes, float pBeepSeconds, float pPauseSeconds) const;   ///< Play multiple beeps.
    //
    void switchOutputOn() const;                                ///< Switch on the buzzer pin.
    void switchOutputOff() const;                               ///< Switch off the buzzer pin.
    //
    void testBuzzerResponse4kHz(bool pTest8kHz = false) const;  ///< Play a beep sequence to test the used buzzer's frequency response.

private:
    const Pin pin;          ///< Pin used to connect the buzzer transistor base (via resistor).
    //
    const float dutyCycle;  ///< Duty cycle (fraction of on state time vs period) used for the beeps.
    //
    const size_t tOn;       ///< "On" time of buzzer pin in microseconds.
    const size_t tOff;      ///< "Off" time of buzzer pin in microseconds.
};

#endif // BUZZER_H

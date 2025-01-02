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

#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "auxil.h"
#include "pins.h"

#include <Arduino.h>

/*!
 * \brief Driver class for push buttons.
 *
 * Provides a push button interface with interrupt handling.
 *
 * \attention You must call setup() before using the class.
 */
class PushButton
{
public:
    using ISRCallbackPtr = Auxil::ISRCallbackPtr;   ///< \copybrief Auxil::ISRCallbackPtr

public:
    PushButton(Pin pPin, bool pActiveLow, size_t pBounceMilliSeconds);  ///< Constructor.
    //
    void setup() const;                                     ///< \copybrief AS3935::setup()
    //
    void enableInterrupt(ISRCallbackPtr pCallback) const;   ///< Attach button pin as Arduino interrupt.
    void disableInterrupt() const;                          ///< Detach Arduino interrupt for button pin.
    //
    void waitPressed() const;   ///< Wait until button is pressed (including debounce delay).
    void waitReleased() const;  ///< Wait until button is released (including debounce delay).
    //
    bool pressed() const;       ///< Check if button is pressed.

private:
    void waitForInputState(uint8_t pState) const;   ///< Wait for a certain state at the button pin (including debounce delay).

private:
    const Pin pin;              ///< Push button's pin.
    //
    const bool activeLow;       ///< Button pressed when pin low.
    const size_t bounceTime;    ///< Bounce time in milliseconds.
};

#endif // PUSHBUTTON_H

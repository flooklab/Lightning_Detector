/*
////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of the firmware of Lightning Detector, an experimental open
//  hardware project for early notification about approaching thunderstorm activity.
//  Copyright (C) 2024 M. Frohne
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

#include "pushbutton.h"

/*!
 * \brief Constructor.
 *
 * \param pPin The push button pin.
 * \param pActiveLow True if active low and false if active high.
 * \param pBounceMilliSeconds Debounce time in milliseconds.
 */
PushButton::PushButton(Pin pPin, bool pActiveLow, size_t pBounceMilliSeconds) :
    pin(pPin),
    activeLow(pActiveLow),
    bounceTime(pBounceMilliSeconds)
{
}

//Public

/*!
 * \copybrief AS3935::setup()
 *
 * Sets the push button pin to input mode with pull-up enabled if the push button is
 * active low and pull-down enabled if the push button is active high, respectively.
 */
void PushButton::setup() const
{
    if (activeLow)
        pinMode(pin, INPUT_PULLUP);
    else
        pinMode(pin, INPUT_PULLDOWN);
}

//

/*!
 * \brief Attach button pin as Arduino interrupt.
 *
 * \param pCallback ISR function to call upon detected interrupt.
 */
void PushButton::enableInterrupt(ISRCallbackPtr pCallback) const
{
    if (activeLow)
    {
        attachInterrupt(digitalPinToInterrupt(pin), pCallback, FALLING);
        pinMode(pin, INPUT_PULLUP);     //Probably not needed (unlike below), but just to be sure
    }
    else
    {
        attachInterrupt(digitalPinToInterrupt(pin), pCallback, RISING);
        pinMode(pin, INPUT_PULLDOWN);   //Fix pull-down configuration, which is somehow changed to pull-up by attachInterrupt()
    }
}

/*!
 * \brief Detach Arduino interrupt for button pin.
 */
void PushButton::disableInterrupt() const
{
    detachInterrupt(digitalPinToInterrupt(pin));
}

//

/*!
 * \brief Wait until button is pressed (including debounce delay).
 */
void PushButton::waitPressed() const
{
    if (activeLow)
        waitForInputState(LOW);
    else
        waitForInputState(HIGH);
}

/*!
 * \brief Wait until button is released (including debounce delay).
 */
void PushButton::waitReleased() const
{
    if (activeLow)
        waitForInputState(HIGH);
    else
        waitForInputState(LOW);
}

//

/*!
 * \brief Check if button is pressed.
 *
 * \note Uses the raw pin state without debouncing.
 *
 * \return True if button is currently pressed.
 */
bool PushButton::pressed() const
{
    if (activeLow)
        return digitalRead(pin) == LOW;
    else
        return digitalRead(pin) == HIGH;
}

//Private

/*!
 * \brief Wait for a certain state at the button pin (including debounce delay).
 *
 * \param pState HIGH or LOW.
 */
void PushButton::waitForInputState(uint8_t pState) const
{
    while (digitalRead(pin) != pState)
        ;
    delay(bounceTime);
    while (digitalRead(pin) != pState)
        ;
}

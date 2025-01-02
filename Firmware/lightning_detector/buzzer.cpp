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

#include "buzzer.h"

#include <algorithm>

/*!
 * \brief Constructor.
 *
 * \param pPin Buzzer pin.
 * \param pFrequency Beep frequency in Hz.
 * \param pDutyCycle Beep signal duty cycle.
 */
Buzzer::Buzzer(Pin pPin, float pFrequency, float pDutyCycle) :
    pin(pPin),
    dutyCycle(pDutyCycle),
    tOn(static_cast<size_t>(1000000./pFrequency*dutyCycle)),
    tOff(static_cast<size_t>(1000000./pFrequency*(1-dutyCycle)))
{
}

//Public

/*!
 * \copybrief AS3935::setup()
 *
 * Sets buzzer control pin to output mode and off.
 */
void Buzzer::setup() const
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

//

/*!
 * \brief Play a single beep.
 *
 * \param pSeconds Beep duration in seconds.
 */
void Buzzer::beepSingle(float pSeconds) const
{
    size_t repetitions = static_cast<size_t>(1000000.*pSeconds/(tOn+tOff));

    for (size_t i = 0; i < repetitions; ++i)
    {
        digitalWrite(pin, HIGH);
        delayMicroseconds(tOn);
        digitalWrite(pin, LOW);
        delayMicroseconds(tOff);
    }
}

/*!
 * \brief Play multiple beeps.
 *
 * Plays \p pTimes beeps of individual length \p pBeepSeconds, separated by \p pTimes-1 pauses of individual length \p pPauseSeconds.
 *
 * \param pTimes Number of beeps.
 * \param pBeepSeconds Beep duration in seconds.
 * \param pPauseSeconds Pause between beeps in seconds.
 */
void Buzzer::beepMulti(size_t pTimes, float pBeepSeconds, float pPauseSeconds) const
{
    if (pTimes == 0)
        return;

    for (size_t i = 0; i < pTimes-1; ++i)
    {
        beepSingle(pBeepSeconds);
        delay(pPauseSeconds);
    }
    beepSingle(pBeepSeconds);
}

//

/*!
 * \brief Switch on the buzzer pin.
 */
void Buzzer::switchOutputOn() const
{
    digitalWrite(pin, HIGH);
}

/*!
 * \brief Switch off the buzzer pin.
 */
void Buzzer::switchOutputOff() const
{
    digitalWrite(pin, LOW);
}

//

/*!
 * \brief Play a beep sequence to test the used buzzer's frequency response.
 *
 * Plays 13 beeps with frequencies around the divided AS3935 resonance frequency (500kHz/128 ~ 4kHz),
 * all separated by steps of 10 cents. In order to achieve a decent calibration precision for the
 * AS3935 using an acoustic buzzer-based tuning procedure you should be able to resolve at least
 * around 3 of those 13 frequencies with the used buzzer model (results may vary, depending on
 * its resonance frequency etc.). Setting \p pTest8kHz to true shifts the test frequency an
 * octave up, which might give different/better results depending on the used buzzer model.
 * Note that the 13 frequencies cover the full +-3.5 percent tuning precision requested
 * by the datasheet. Therefore there does not seem to be a good reason to waste more
 * time on maximizing the resolvable frequencies than absolutely necessary.
 *
 * \param pTest8kHz Do the test an octave higher (around 500kHz/64 ~ 8kHz).
 */
void Buzzer::testBuzzerResponse4kHz(bool pTest8kHz) const
{
    //Try 13 frequencies in steps of 10 cents around target frequency
    constexpr float fs[] = {3773, 3795, 3817, 3839, 3861, 3884, 3906, 3929, 3952, 3975, 3998, 4021, 4044};

    for (size_t i = 0; i < std::extent<decltype(fs)>::value; ++i)
    {
        float cFr = fs[i];

        if (pTest8kHz)
            cFr *= 2.;

        size_t lOn = static_cast<size_t>(1000000./cFr*dutyCycle);
        size_t lOff = static_cast<size_t>(1000000./cFr*(1-dutyCycle));

        size_t reps = static_cast<size_t>(1000000.*0.6/(lOn+lOff));

        for (size_t i = 0; i < reps; ++i)
        {
            digitalWrite(pin, HIGH);
            delayMicroseconds(lOn);
            digitalWrite(pin, LOW);
            delayMicroseconds(lOff);
        }

        delay(200);
    }
}

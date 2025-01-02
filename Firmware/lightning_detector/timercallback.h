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

#ifndef TIMERCALLBACK_H
#define TIMERCALLBACK_H

#include "auxil.h"

#include <Arduino.h>

#include <nrf_nvic.h>
#include <nrf_rtc.h>

/*!
 * \brief RTC timer interface with callback functionality.
 *
 * Provides a low precision RTC hardware timer for triggering a callback function when the set time has elapsed.
 *
 * Only a single TimerCallback instance can be used at a time.
 *
 * \attention You must call setup() before using the class.
 */
class TimerCallback
{
public:
    using ISRCallbackPtr = Auxil::ISRCallbackPtr;   ///< \copybrief Auxil::ISRCallbackPtr

public:
    TimerCallback(uint32_t pTimeoutCtr125ms, ISRCallbackPtr pCallback);     ///< Constructor.
    //
    void setup() const;         ///< Configure RTC timer peripheral and set instance callback as interrupt vector.
    //
    void startTimer() const;    ///< Start the timer.
    uint32_t stopTimer() const; ///< Stop the timer.

private:
    static void staticISR();    ///< Interrupt service routine (stage 1, static).
    void isr() const;           ///< Interrupt service routine (stage 2; instance-bound).

private:
    static constexpr uint32_t timerIdx = 2;     ///< Index of internally used RTC instance.
    //
    static constexpr IRQn_Type irqTypes[] = {RTC0_IRQn, RTC1_IRQn, RTC2_IRQn};  ///< IRQ types to be used for available RTC instances.
    static constexpr IRQn_Type irqType = irqTypes[timerIdx];                    ///< IRQ type for used RTC instance.
    //
    static NRF_RTC_Type *const timerTypes[];    ///< Available RTC instances.
    static NRF_RTC_Type *const timer;           ///< Used RTC instance.

private:
    const uint32_t timeoutCtr125ms;         ///< Timer timeout measured in steps of 125ms.
    //
    ISRCallbackPtr callback;                ///< Callback function to call in case of timer timeout.
    //
    static const TimerCallback* instance;   ///< Currently used TimerCallback instance to identify proper callback function.
};

#endif // TIMERCALLBACK_H

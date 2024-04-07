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

#include "timercallback.h"

NRF_RTC_Type *const TimerCallback::timerTypes[] = {NRF_RTC0, NRF_RTC1, NRF_RTC2};
NRF_RTC_Type *const TimerCallback::timer = TimerCallback::timerTypes[TimerCallback::timerIdx];
//
const TimerCallback* TimerCallback::instance = nullptr;

//

/*!
 * \brief Constructor.
 *
 * \param pTimeoutCtr125ms Timer timeout in steps of 125ms.
 * \param pCallback ISR function to call upon timer timeout. See also isr().
 */
TimerCallback::TimerCallback(uint32_t pTimeoutCtr125ms, ISRCallbackPtr pCallback) :
    timeoutCtr125ms(pTimeoutCtr125ms),
    callback(pCallback)
{
}

//Public

/*!
 * \brief Configure RTC timer peripheral and set instance callback as interrupt vector.
 */
void TimerCallback::setup() const
{
    instance = this;

    sd_nvic_DisableIRQ(irqType);

    timer->TASKS_STOP = 1;
    timer->TASKS_CLEAR = 1;

    timer->PRESCALER = 4095;    //8Hz
    timer->CC[0] = timeoutCtr125ms;
    timer->INTENSET = NRF_RTC_INT_COMPARE0_MASK;

    NVIC_SetVector(irqType, reinterpret_cast<uint32_t>(&TimerCallback::staticISR));
}

//

/*!
 * \brief Start the timer.
 */
void TimerCallback::startTimer() const
{
    sd_nvic_EnableIRQ(irqType);
    timer->TASKS_START = 1;
}

/*!
 * \brief Stop the timer.
 *
 * Also clears the counter.
 *
 * \return Raw number of timer counts (of 125ms length each).
 */
uint32_t TimerCallback::stopTimer() const
{
    timer->TASKS_STOP = 1;

    uint32_t ctrVal125ms = timer->COUNTER;

    timer->TASKS_CLEAR = 1;
    sd_nvic_DisableIRQ(irqType);

    return ctrVal125ms;
}

//Private

/*!
 * \brief Interrupt service routine (stage 1, static).
 *
 * Triggered by the timer timeout.
 *
 * Just forwards to isr() using the instance set via calling setup().
 */
void TimerCallback::staticISR()
{
    if (instance != nullptr)
        instance->isr();
}

/*!
 * \brief Interrupt service routine (stage 2; instance-bound).
 *
 * Called by staticISR() when the timer times out.
 *
 * Stops the timer, clears the counter and calls the actual callback function that was passed to TimerCallback().
 */
void TimerCallback::isr() const
{
    sd_nvic_DisableIRQ(irqType);

    if (timer->EVENTS_COMPARE[0] == 1)
    {
        timer->TASKS_STOP = 1;
        timer->TASKS_CLEAR = 1;

        timer->EVENTS_COMPARE[0] = 0;

        callback();
    }

    sd_nvic_EnableIRQ(irqType);
}

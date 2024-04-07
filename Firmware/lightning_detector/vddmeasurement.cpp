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

#include "vddmeasurement.h"

bool VDDMeasurement::setUp = false;
//
volatile nrf_saadc_value_t VDDMeasurement::adcBuffer[1] = {static_cast<nrf_saadc_value_t>(0)};

//Public

/*!
 * \brief Configure SAADC peripheral for measurement of VDD.
 *
 * Configures and auto-calibrates the SAADC peripheral.
 *
 * Returns immediately, if already called before.
 */
void VDDMeasurement::setup()
{
    if (setUp)
        return;

    NRF_SAADC->ENABLE = 0;

    NRF_SAADC->RESOLUTION = NRF_SAADC_RESOLUTION_10BIT;

    NRF_SAADC->OVERSAMPLE = NRF_SAADC_OVERSAMPLE_4X;

    NRF_SAADC->CH[0].PSELP = NRF_SAADC_INPUT_VDD;
    NRF_SAADC->CH[0].PSELN = NRF_SAADC_INPUT_DISABLED;

    NRF_SAADC->CH[0].CONFIG = ((NRF_SAADC_RESISTOR_DISABLED << SAADC_CH_CONFIG_RESP_Pos) |
                               (NRF_SAADC_RESISTOR_DISABLED << SAADC_CH_CONFIG_RESN_Pos) |
                               (NRF_SAADC_GAIN1_6 << SAADC_CH_CONFIG_GAIN_Pos) |
                               (NRF_SAADC_REFERENCE_INTERNAL << SAADC_CH_CONFIG_REFSEL_Pos) |
                               (NRF_SAADC_ACQTIME_40US << SAADC_CH_CONFIG_TACQ_Pos) |
                               (NRF_SAADC_MODE_SINGLE_ENDED << SAADC_CH_CONFIG_MODE_Pos) |
                               (NRF_SAADC_BURST_DISABLED << SAADC_CH_CONFIG_BURST_Pos));

    NRF_SAADC->RESULT.MAXCNT = 1;
    NRF_SAADC->RESULT.PTR = reinterpret_cast<uint32_t>(&adcBuffer);

    //Calibrate

    NRF_SAADC->ENABLE = 1;

    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
    while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0)
        ;
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    while (NRF_SAADC->STATUS == 1)
        ;

    NRF_SAADC->ENABLE = 0;

    setUp = true;
}

//

/*!
 * \brief Measure the current value of VDD.
 *
 * See also takeADCSample().
 *
 * \return Measured VDD in Volt (or 0 if setup() was never called).
 */
float VDDMeasurement::measureVoltage()
{
    if (!setUp)
        return 0;

    return (static_cast<float>(takeADCSample()) * 0.003515625);
}

//Private

/*!
 * \brief Take an SAADC sample of VDD.
 *
 * Takes a four-fold "oversampled" (i.e. averaged) ADC sample of the muxed VDD voltage.
 *
 * Return value needs to be converted to a voltage using the SAADC settings from setup().
 *
 * \return Raw ADC sample buffer value.
 */
int16_t VDDMeasurement::takeADCSample()
{
    NRF_SAADC->ENABLE = 1;

    NRF_SAADC->EVENTS_STARTED = 0;
    NRF_SAADC->EVENTS_DONE = 0;
    NRF_SAADC->EVENTS_RESULTDONE = 0;
    NRF_SAADC->EVENTS_END = 0;

    NRF_SAADC->TASKS_START = 1;
    while (NRF_SAADC->EVENTS_STARTED == 0)
        ;
    NRF_SAADC->EVENTS_STARTED = 0;

    for (size_t i = 0; i < 4; ++i)
    {
        NRF_SAADC->TASKS_SAMPLE = 1;
        while (NRF_SAADC->EVENTS_DONE == 0)
            ;
        NRF_SAADC->EVENTS_DONE = 0;
    }

    while (NRF_SAADC->EVENTS_RESULTDONE == 0)
        ;
    NRF_SAADC->EVENTS_RESULTDONE = 0;

    while (NRF_SAADC->EVENTS_END == 0)
        ;
    NRF_SAADC->EVENTS_END = 0;

    int16_t bufferVal = static_cast<int16_t>(adcBuffer[0]);

    NRF_SAADC->TASKS_STOP = 1;
    while (NRF_SAADC->EVENTS_STOPPED == 0)
        ;
    NRF_SAADC->EVENTS_STOPPED = 0;

    NRF_SAADC->ENABLE = 0;

    return bufferVal;
}

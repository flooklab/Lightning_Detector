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

#ifndef VDDMEASUREMENT_H
#define VDDMEASUREMENT_H

#include <Arduino.h>

#include <nrf_saadc.h>

/*!
 * \brief VDD measurement interface using Arduino's internal ADC.
 *
 * Provides a static function to measure VDD using the successive-approximation ADC of the Arduino.
 *
 * \attention You must call setup() before using the class.
 */
class VDDMeasurement
{
public:
    VDDMeasurement() = delete;      ///< Deleted constructor.
    //
    static void setup();            ///< Configure SAADC peripheral for measurement of VDD.
    //
    static float measureVoltage();  ///< Measure the current value of VDD.

private:
    static int16_t takeADCSample(); ///< Take an SAADC sample of VDD.

private:
    static bool setUp;                              ///< SAADC is configured.
    //
    static volatile nrf_saadc_value_t adcBuffer[1]; ///< Buffer for SAADC result.
};

#endif // VDDMEASUREMENT_H

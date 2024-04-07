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

#ifndef SPICONFIG_H
#define SPICONFIG_H

#include <Arduino.h>

/*!
 * \brief Enable the SPI bus on request.
 *
 * This is a static class to abstract enabling of SPI, so that SPI gets enabled
 * \e once as soon as some device driver requests it and keeps it disabled otherwise.
 */
class SPIConfig
{
public:
    SPIConfig() = delete;           ///< Deleted constructor.
    //
    static uint32_t getSPISpeed();  ///< Get the pre-defined clock frequency.
    //
    static void enableSPI();        ///< Enable the SPI bus.

private:
    static constexpr uint32_t spiSpeed = 200000;    ///< SPI clock freq in Hz; avoid interference of SPI signal with 500kHz antenna band.
    //
    static bool spiEnabled;     ///< SPI bus has been enabled.
};

#endif // SPICONFIG_H

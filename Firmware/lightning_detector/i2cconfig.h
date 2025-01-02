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

#ifndef I2CCONFIG_H
#define I2CCONFIG_H

#include <Arduino.h>

/*!
 * \brief Enable the I2C bus on request.
 *
 * This is a static class to abstract enabling of I2C, so that I2C gets enabled
 * \e once as soon as some device driver requests it and keeps it disabled otherwise.
 */
class I2CConfig
{
public:
    I2CConfig() = delete;       ///< Deleted constructor.
    //
    static void enableI2C();    ///< Enable the I2C bus.

private:
    static constexpr uint32_t i2cSpeed = 100000;    ///< \brief I2C clock freq in Hz; avoid interference
                                                    ///<        of I2C signal with 500kHz antenna band.
    //
    static bool i2cEnabled;     ///< I2C bus has been enabled.
};

#endif // I2CCONFIG_H

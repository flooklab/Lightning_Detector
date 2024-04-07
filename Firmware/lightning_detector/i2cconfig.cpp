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

#include "i2cconfig.h"

#include <Wire.h>

constexpr uint32_t I2CConfig::i2cSpeed;
//
bool I2CConfig::i2cEnabled = false;

//Public

/*!
 * \brief Enable the I2C bus.
 *
 * Enables the I2C bus if this function has not been called before.
 */
void I2CConfig::enableI2C()
{
    if (i2cEnabled)
        return;

    NRF_TWI0->ENABLE = 1;
    Wire.begin();
    Wire.setClock(i2cSpeed);

    i2cEnabled = true;
}

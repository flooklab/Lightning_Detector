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

#include "abstractspidisplay.h"

#include "spiconfig.h"
#include "i2cconfig.h"

/*!
 * \brief Constructor.
 *
 * \param pWidth Width of the display in pixels.
 * \param pHeight Height of the display in pixels.
 * \param pSPISettings Settings to use for the SPI transactions.
 * \param pIOExpanderI2CAddr I2C address of the used IO expander chip.
 */
AbstractSPIDisplay::AbstractSPIDisplay(size_t pWidth, size_t pHeight, SPISettings pSPISettings, uint8_t pIOExpanderI2CAddr) :
    AbstractDisplay(pWidth, pHeight),
    spiSettings(pSPISettings),
    ioExpanderI2CAddr(pIOExpanderI2CAddr)
{
}

//Public

/*!
 * \copybrief AbstractDisplay::setup()
 *
 * Calls SPIConfig::enableSPI() and I2CConfig::enableI2C().
 */
void AbstractSPIDisplay::setup() const
{
    SPIConfig::enableSPI();
    I2CConfig::enableI2C();
}
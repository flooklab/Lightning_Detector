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

#ifndef ABSTRACTSPIDISPLAY_H
#define ABSTRACTSPIDISPLAY_H

#include "abstractdisplay.h"

#include "as3935.h"
#include "auxil.h"

#include <Arduino.h>
#include <SPI.h>

/*!
 * \brief SPI-based display driver base class.
 *
 * Specializes the abstract display driver interface AbstractDisplay (see also there!) by adding generic SPI bus
 * functionality. Use this class instead of AbstractDisplay to implement a driver class for an SPI-based display.
 *
 * To use the SPI bus for the display you will need to have an IO expander connected to the I2C bus to at least provide another
 * IO pin for the corresponding 'chip select' signal. The IO expander functionality must be implemented by your driver, too.
 * The I2C address of the IO expander must be already passed to this class' constructor (see AbstractSPIDisplay()).
 */
class AbstractSPIDisplay : public AbstractDisplay
{
public:
    AbstractSPIDisplay(size_t pWidth, size_t pHeight, SPISettings pSPISettings, uint8_t pIOExpanderI2CAddr);    ///< Constructor.
    virtual ~AbstractSPIDisplay() = default;    ///< Default destructor.
    //
    void setup() const override;                ///< \copybrief AbstractDisplay::setup()
    //
    void init() const override = 0;             ///< \copybrief AbstractDisplay::init()
    void sleep() const override = 0;            ///< \copybrief AbstractDisplay::sleep()
    //
    void clear() override = 0;                  ///< \copybrief AbstractDisplay::clear()
    void update(size_t pNumLightnings, float pLightningRate, uint8_t pStormDist, float pBatteryPercentage, float pBatteryVoltage,
                float pRunTimeHours, Auxil::RunMode pMode, bool pSerialEnabled, AS3935::InterruptType pLastInterrupt) override = 0;
                                                ///< \copydoc AbstractDisplay::update()

private:
    virtual void selectChip() const = 0;        ///< Enable display's SPI 'chip select' signal.
    virtual void deselectChip() const = 0;      ///< Disable display's SPI 'chip select' signal.

protected:
    const SPISettings spiSettings;      ///< Settings for SPI transactions.
    //
    const uint8_t ioExpanderI2CAddr;    ///< I2C address for required IO expander chip.
};

#endif // ABSTRACTSPIDISPLAY_H

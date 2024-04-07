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

#ifndef DISPLAY_DUMMY_H
#define DISPLAY_DUMMY_H

#include "abstractdisplay.h"

#include "as3935.h"
#include "auxil.h"

#include <Arduino.h>

#define DISPLAY_TYPE Display_Dummy

/*!
 * \brief Dummy display driver.
 *
 * Use this dummy display driver if no display is going to be connected.
 */
class DISPLAY_TYPE : public AbstractDisplay
{
public:
    DISPLAY_TYPE();                     ///< Constructor.
    //
    void setup() const override final;  ///< \copybrief AbstractDisplay::setup()
    //
    void init() const override final;   ///< \copybrief AbstractDisplay::init()
    void sleep() const override final;  ///< \copybrief AbstractDisplay::sleep()
    //
    void clear() override final;        ///< \copybrief AbstractDisplay::clear()
    void update(size_t pNumLightnings, float pLightningRate, uint8_t pStormDist, float pBatteryPercentage, float pBatteryVoltage,
                float pRunTimeHours, Auxil::RunMode pMode, bool pSerialEnabled, AS3935::InterruptType pLastInterrupt) override final;
                                        ///< \copybrief AbstractDisplay::update()

private:
    void setPixel(size_t pX, size_t pY, uint8_t pR, uint8_t pG, uint8_t pB) override final;     ///< \copybrief AbstractDisplay::setPixel()
};

#endif // DISPLAY_DUMMY_H

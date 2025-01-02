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

#include "display_dummy.h"

/*!
 * \brief Constructor.
 *
 * Initializes parent AbstractDisplay with zero width and height.
 */
DISPLAY_TYPE::DISPLAY_TYPE() :
    AbstractDisplay(0, 0)
{
}

//Public

/*!
 * \copybrief AbstractDisplay::setup()
 *
 * \note Does nothing.
 */
void DISPLAY_TYPE::setup() const
{
}

//

/*!
 * \copybrief AbstractDisplay::init()
 *
 * \note Does nothing.
 */
void DISPLAY_TYPE::init() const
{
}

/*!
 * \copybrief AbstractDisplay::sleep()
 *
 * \note Does nothing.
 */
void DISPLAY_TYPE::sleep() const
{
}

//

/*!
 * \copybrief AbstractDisplay::clear()
 *
 * \note Does nothing.
 */
void DISPLAY_TYPE::clear()
{
}

/*!
 * \copybrief AbstractDisplay::update()
 *
 * \note Does nothing.
 *
 * \param pNumLightnings Accumulated number of lightnings.
 * \param pLightningRate Current lightning rate as number of lightnings per minute.
 * \param pStormDist AS3935-internal estimate of storm distance in kilometers.
 * \param pBatteryPercentage Fraction of available battery charge as percentage from 0 to 100.
 * \param pBatteryVoltage Measured battery voltage.
 * \param pRunTimeHours Estimated run time in hours since last reset.
 * \param pMode Run mode.
 * \param pSerialEnabled Serial connection enabled.
 * \param pLastInterrupt Last interrupt request type from AS3935.
 */
void DISPLAY_TYPE::update(size_t pNumLightnings, float pLightningRate, uint8_t pStormDist, float pBatteryPercentage, float pBatteryVoltage,
                          float pRunTimeHours, Auxil::RunMode pMode, bool pSerialEnabled, AS3935::InterruptType pLastInterrupt)
{
}

//Private

/*!
 * \copybrief AbstractDisplay::setPixel()
 *
 * \note Does nothing.
 *
 * \param pX Display's x-coordinate.
 * \param pY Display's y-coordinate.
 * \param pR Red pixel value from 0 to 255.
 * \param pG Green pixel value from 0 to 255.
 * \param pB Blue pixel value from 0 to 255.
 */
void DISPLAY_TYPE::setPixel(size_t pX, size_t pY, uint8_t pR, uint8_t pG, uint8_t pB)
{
}

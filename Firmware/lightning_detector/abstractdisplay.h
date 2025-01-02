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

#ifndef ABSTRACTDISPLAY_H
#define ABSTRACTDISPLAY_H

#include "as3935.h"
#include "auxil.h"
#include "graphics.h"

#include <Arduino.h>

/*!
 * \brief Display driver base class.
 *
 * Provides an abstract display driver interface for use with the display functionality of
 * the main Sketch file. Use this interface to implement an actual display driver class.
 *
 * %Graphics functionality is provided by an instance of the Graphics class, which the driver instance must be
 * linked to in the driver's constructor (see Graphics::setDisplay())! Graphics then uses setPixel() to write
 * to the display driver's graphics buffer (buffer to be implemented in the derived driver class as well!).
 */
class AbstractDisplay
{
public:
    AbstractDisplay(size_t pWidth, size_t pHeight); ///< Constructor.
    virtual ~AbstractDisplay() = default;           ///< Default destructor.
    //
    virtual void setup() const = 0;                 ///< \copybrief AS3935::setup()
    //
    virtual void init() const = 0;                  ///< Initialize the display.
    virtual void sleep() const = 0;                 ///< Send the display to sleep/off mode.
    //
    virtual void clear() = 0;                       ///< Clear the display content.
    /*!
     * \brief Show the (updated) useful information on the display.
     *
     * \note The overall formatting and which parts of the passed information are shown is up to the specific implementation.
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
    virtual void update(size_t pNumLightnings, float pLightningRate, uint8_t pStormDist, float pBatteryPercentage, float pBatteryVoltage,
                        float pRunTimeHours, Auxil::RunMode pMode, bool pSerialEnabled, AS3935::InterruptType pLastInterrupt) = 0;

private:
    /*!
     * \brief Overwrite the color of a specific pixel in the display buffer.
     *
     * \param pX Display's x-coordinate.
     * \param pY Display's y-coordinate.
     * \param pR Red pixel value from 0 to 255.
     * \param pG Green pixel value from 0 to 255.
     * \param pB Blue pixel value from 0 to 255.
     */
    virtual void setPixel(size_t pX, size_t pY, uint8_t pR, uint8_t pG, uint8_t pB) = 0;

private:
    /*!
     * \copybrief Graphics::set
     *
     * See Graphics::set().
     */
    friend void Graphics::set(int, int, uint8_t, uint8_t, uint8_t);

protected:
    Graphics graphics;  ///< Graphics instance for performing all graphics operations on the display buffer.
};

#endif // ABSTRACTDISPLAY_H

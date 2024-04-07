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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "arduinographics.h"

#include <Arduino.h>

class AbstractDisplay;

/*!
 * \brief Simple graphics operations for display drivers based on AbstractDisplay.
 *
 * To be used by AbstractDisplay to enable graphics functionality for the display drivers.
 *
 * This Graphics class expects the actual derived display driver class to implement the
 * function AbstractDisplay::setPixel(). The driver instance must be set via setDisplay().
 *
 * See ArduinoGraphics for the provided graphics functionality itself.
 */
class Graphics : public ArduinoGraphics
{
public:
    Graphics(size_t pWidth, size_t pHeight);                            ///< Constructor.
    //
    void set(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;   ///< \copybrief ArduinoGraphics::set()
    //
    void setDisplay(AbstractDisplay* pDisplayPtr);                      ///< Set the used display driver instance.

private:
    AbstractDisplay* displayPtr;    ///< Used display driver instance.
};

#endif // GRAPHICS_H

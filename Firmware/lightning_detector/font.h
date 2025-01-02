/*
////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of the firmware of Lightning Detector, an experimental open
//  hardware project for early notification about approaching thunderstorm activity.
//  This file was derived from the ArduinoGraphics library.
//  Copyright (C) 2019 Arduino SA
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

#ifndef FONT_H
#define FONT_H

#include <Arduino.h>

/*!
 * \brief ASCII font object for use with ArduinoGraphics.
 *
 * Consists of an array of monospaced, monochrome font character bitmaps.
 *
 * The font width is limited to maximally 16 pixels.
 */
struct Font
{
    const size_t width;     ///< %Font width in pixels.
    const size_t height;    ///< %Font height in pixels.
    const uint16_t** data;  ///< \brief %Font data as array of bitmaps.
                            ///  \details Each bitmap is an array of whole lines
                            ///  with each line being represented by a \p uint16_t.
                            ///  The MSB corresponds to the leftmost pixel of a line.
                            ///  Line index 0 corresponds to the topmost line.
};

#endif // FONT_H

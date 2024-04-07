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

#include "graphics.h"

#include "abstractdisplay.h"

/*!
 * \brief Constructor.
 *
 * \note You need to set a display driver instance using setDisplay() in the following.
 *
 * \param pWidth Bitmap width in pixels.
 * \param pHeight Bitmap height in pixels.
 */
Graphics::Graphics(size_t pWidth, size_t pHeight) :
    ArduinoGraphics(pWidth, pHeight),
    displayPtr(nullptr)
{
}

//Public

/*!
 * \copybrief ArduinoGraphics::set()
 *
 * Just forwards to AbstractDisplay::setPixel() using the set driver instance (see setDisplay()).
 *
 * \param x Bitmap's x-coordinate.
 * \param y Bitmap's y-coordinate.
 * \param r Red pixel value from 0 to 255.
 * \param g Green pixel value from 0 to 255.
 * \param b Blue pixel value from 0 to 255.
 */
void Graphics::set(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (displayPtr != nullptr)
        displayPtr->setPixel(x, y, r, g, b);
}

//

/*!
 * \brief Set the used display driver instance.
 *
 * \param pDisplayPtr Pointer to the used driver instance.
 */
void Graphics::setDisplay(AbstractDisplay* pDisplayPtr)
{
    displayPtr = pDisplayPtr;
}

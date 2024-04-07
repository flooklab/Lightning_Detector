/*
////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of the firmware of Lightning Detector, an experimental open
//  hardware project for early notification about approaching thunderstorm activity.
//  This file was derived from the ArduinoGraphics library.
//  Copyright (C) 2019 Arduino SA
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

#ifndef ARDUINOGRAPHICS_H
#define ARDUINOGRAPHICS_H

#include "font.h"

#include <Arduino.h>

/*!
 * \brief Perform simple graphics operations on a bitmap image.
 *
 * Provides elementary graphics operations, including text writing based on the Font class,
 * to be applied on up to 24-bit RGB bitmaps. The graphics buffer format is a priori
 * arbitrary because it must be implemented by deriving from this abstract base class
 * and implementing the set(int, int, uint8_t, uint8_t, uint8_t) function anyway.
 */
class ArduinoGraphics
{
public:
    ArduinoGraphics(int width, int height); ///< Constructor.
    virtual ~ArduinoGraphics() = default;   ///< Default destructor.
    //
    int begin();                            ///< Reset painting properties.
    //
    int width() const;                      ///< Get the image width.
    int height() const;                     ///< Get the image height.
    uint32_t background() const;            ///< Get the current background color.
    //
    void background(uint8_t r, uint8_t g, uint8_t b);   ///< Set the background color.
    void background(uint32_t color);                    ///< Set the background color.
    void clear();                                       ///< Reset all pixels to the background color.
    void fill(uint8_t r, uint8_t g, uint8_t b);         ///< Enable filling and set fill color.
    void fill(uint32_t color);                          ///< Enable filling and set fill color.
    void noFill();                                      ///< Disable filling.
    void stroke(uint8_t r, uint8_t g, uint8_t b);       ///< Enable stroke and set stroke color.
    void stroke(uint32_t color);                        ///< Enable stroke and set stroke color.
    void noStroke();                                    ///< Disable stroke.
    //
    void circle(int x, int y, int diameter);            ///< Draw a circle.
    void ellipse(int x, int y, int width, int height);  ///< Draw an ellipse.
    void line(int x1, int y1, int x2, int y2);          ///< Draw a line.
    void point(int x, int y);                           ///< Draw a point.
    void rect(int x, int y, int width, int height);     ///< Draw a rectangle.
    //
    void text(const char* str, int x = 0, int y = 0);   ///< Draw text.
    void text(const String& str, int x = 0, int y = 0); ///< Draw text.
    void textFont(const Font* which);                   ///< Set the text font.
    //
    int textFontWidth() const;                          ///< Get the current font's character width.
    int textFontHeight() const;                         ///< Get the current font's character height.
    //
    /*!
     * \brief Set the color of a pixel.
     *
     * \param x Bitmap's x-coordinate.
     * \param y Bitmap's y-coordinate.
     * \param r Red pixel value from 0 to 255.
     * \param g Green pixel value from 0 to 255.
     * \param b Blue pixel value from 0 to 255.
     */
    virtual void set(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    void set(int x, int y, uint32_t color);                                 ///< Set the color of a pixel.
    //
    void beginText(int x = 0, int y = 0);                           ///< Clear text buffer and set cursor position.
    void beginText(int x, int y, uint8_t r, uint8_t g, uint8_t b);  ///< Clear text buffer, set cursor position and text color.
    void beginText(int x, int y, uint32_t color);                   ///< Clear text buffer, set cursor position and text color.
    void endText();                                                 ///< Draw the text currently in text buffer and clear text buffer.

private:
    void bitmap(const uint16_t* data, int x, int y, int width, int height); ///< Insert a raw monochrome bitmap.
    //
    void lineLow(int x1, int y1, int x2, int y2);   ///< Draw flat oblique lines with angle below 45 degrees to x-axis.
    void lineHigh(int x1, int y1, int x2, int y2);  ///< Draw steep oblique lines with angle at least 45 degrees to x-axis.
    //
    static uint8_t colorR(uint32_t color);  ///< Extract red color value from RGB color value.
    static uint8_t colorG(uint32_t color);  ///< Extract green color value from RGB color value.
    static uint8_t colorB(uint32_t color);  ///< Extract blue color value from RGB color value.

private:
    int _width;             ///< Image width.
    int _height;            ///< Image height.
    const Font* _font;      ///< Currently used font.
    //
    bool _fill;             ///< Filling enabled.
    bool _stroke;           ///< Stroke enabled.
    uint8_t _backgroundR;   ///< Background color red value.
    uint8_t _backgroundG;   ///< Background color green value.
    uint8_t _backgroundB;   ///< Background color blue value.
    uint8_t _fillR;         ///< Fill color red value.
    uint8_t _fillG;         ///< Fill color green value.
    uint8_t _fillB;         ///< Fill color blue value.
    uint8_t _strokeR;       ///< Stroke color red value.
    uint8_t _strokeG;       ///< Stroke color green value.
    uint8_t _strokeB;       ///< Stroke color blue value.
    //
    String _textBuffer;     ///< Text writing buffer.
    uint8_t _textR;         ///< Text color red value.
    uint8_t _textG;         ///< Text color green value.
    uint8_t _textB;         ///< Text color blue value.
    int _textX;             ///< Text cursor x-position.
    int _textY;             ///< Text cursor y-position.
};

#endif // ARDUINOGRAPHICS_H

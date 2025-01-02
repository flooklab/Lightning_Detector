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

#include "arduinographics.h"

/*!
 * \brief Constructor.
 *
 * Starts with stroke and fill disabled and \e without a font being set.
 *
 * \param width Bitmap width in pixels.
 * \param height Bitmap height in pixels.
 */
ArduinoGraphics::ArduinoGraphics(int width, int height) :
    _width(width),
    _height(height),
    _font(nullptr),
    _fill(false),
    _stroke(false),
    _backgroundR(0),
    _backgroundG(0),
    _backgroundB(0),
    _fillR(0),
    _fillG(0),
    _fillB(0),
    _strokeR(0),
    _strokeG(0),
    _strokeB(0),
    _textBuffer(""),
    _textR(0),
    _textG(0),
    _textB(0),
    _textX(0),
    _textY(0)
{
}

//Public

/*!
 * \brief Reset painting properties.
 *
 * Sets background color to black, disables fill and stroke.
 *
 * \return 1.
 */
int ArduinoGraphics::begin()
{
    background(0, 0, 0);
    noFill();
    noStroke();

    return 1;
}

//

/*!
 * \brief Get the image width.
 *
 * \return Width in pixels.
 */
int ArduinoGraphics::width() const
{
    return _width;
}

/*!
 * \brief Get the image height.
 *
 * \return Height in pixels.
 */
int ArduinoGraphics::height() const
{
    return _height;
}

/*!
 * \brief Get the current background color.
 *
 * \return RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
uint32_t ArduinoGraphics::background() const
{
    uint32_t bg = static_cast<uint32_t>(static_cast<uint32_t>(_backgroundR << 16) |
                                        static_cast<uint32_t>(_backgroundG <<  8) |
                                        static_cast<uint32_t>(_backgroundB <<  0));
    return bg;
}

//

/*!
 * \brief Set the background color.
 *
 * This color value is used by clear() and bitmap().
 *
 * \param r Red color value from 0 to 255.
 * \param g Green color value from 0 to 255.
 * \param b Blue color value from 0 to 255.
 */
void ArduinoGraphics::background(uint8_t r, uint8_t g, uint8_t b)
{
    _backgroundR = r;
    _backgroundG = g;
    _backgroundB = b;
}

/*!
 * \brief Set the background color.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
void ArduinoGraphics::background(uint32_t color)
{
    background(colorR(color), colorG(color), colorB(color));
}

/*!
 * \brief Reset all pixels to the background color.
 */
void ArduinoGraphics::clear()
{
    for (int x = 0; x < _width; x++)
        for (int y = 0; y < _height; y++)
            set(x, y, _backgroundR, _backgroundG, _backgroundB);
}

/*!
 * \brief Enable filling and set fill color.
 *
 * \param r Red color value from 0 to 255.
 * \param g Green color value from 0 to 255.
 * \param b Blue color value from 0 to 255.
 */
void ArduinoGraphics::fill(uint8_t r, uint8_t g, uint8_t b)
{
    _fill = true;
    _fillR = r;
    _fillG = g;
    _fillB = b;
}

/*!
 * \brief Enable filling and set fill color.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
void ArduinoGraphics::fill(uint32_t color)
{
    fill(colorR(color), colorG(color), colorB(color));
}

/*!
 * \brief Disable filling.
 */
void ArduinoGraphics::noFill()
{
    _fill = false;
}

/*!
 * \brief Enable stroke and set stroke color.
 *
 * \param r Red color value from 0 to 255.
 * \param g Green color value from 0 to 255.
 * \param b Blue color value from 0 to 255.
 */
void ArduinoGraphics::stroke(uint8_t r, uint8_t g, uint8_t b)
{
    _stroke = true;
    _strokeR = r;
    _strokeG = g;
    _strokeB = b;
}

/*!
 * \brief Enable stroke and set stroke color.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
void ArduinoGraphics::stroke(uint32_t color)
{
    stroke(colorR(color), colorG(color), colorB(color));
}

/*!
 * \brief Disable stroke.
 */
void ArduinoGraphics::noStroke()
{
    _stroke = false;
}

//

/*!
 * \brief Draw a circle.
 *
 * See also ellipse().
 *
 * \param x Circle center x-coordinate.
 * \param y Circle center y-coordinate.
 * \param diameter Circle diameter.
 */
void ArduinoGraphics::circle(int x, int y, int diameter)
{
    ellipse(x, y, diameter, diameter);
}

/*!
 * \brief Draw an ellipse.
 *
 * \param x Ellipse center x-coordinate.
 * \param y Ellipse center y-coordinate.
 * \param width Ellipse width.
 * \param height Ellipse height.
 */
void ArduinoGraphics::ellipse(int x, int y, int width, int height)
{
    if (!_stroke && !_fill)
        return;

    int32_t a = width / 2;
    int32_t b = height / 2;
    int64_t a2 = a * a;
    int64_t b2 = b * b;
    int64_t i, j;

    if (_fill)
    {
        for (j = -b; j <= b; j++)
        {
            for (i = -a; i <= a; i++)
            {
                if (i*i*b2 + j*j*a2 <= a2*b2)
                    set(x + i, y + j, _fillR, _fillG, _fillB);
            }
        }
    }

    if (_stroke)
    {
        int x_val, y_val;
        for (i = -a; i <= a; i++)
        {
            y_val = b * sqrt(1 - static_cast<double>(i*i) / a2);
            set(x + i, y + y_val, _strokeR, _strokeG, _strokeB);
            set(x + i, y - y_val, _strokeR, _strokeG, _strokeB);
        }
        for (j = -b; j <= b; j++)
        {
            x_val = a * sqrt(1 - static_cast<double>(j*j) / b2);
            set(x + x_val, y + j, _strokeR, _strokeG, _strokeB);
            set(x - x_val, y + j, _strokeR, _strokeG, _strokeB);
        }
    }
}

/*!
 * \brief Draw a line.
 *
 * \note Uses lineLow() and lineHigh() for oblique lines.
 *
 * \param x1 Start point x-coordinate.
 * \param y1 Start point y-coordinate.
 * \param x2 End point x-coordinate.
 * \param y2 End point y-coordinate.
 */
void ArduinoGraphics::line(int x1, int y1, int x2, int y2)
{
    if (!_stroke)
        return;

    if (x1 == x2)
    {
        for (int y = y1; y <= y2; y++)
            set(x1, y, _strokeR, _strokeG, _strokeB);
    }
    else if (y1 == y2)
    {
        for (int x = x1; x <= x2; x++)
            set(x, y1, _strokeR, _strokeG, _strokeB);
    }
    else if (abs(y2 - y1) < abs(x2 - x1))
    {
        if (x1 > x2)
            lineLow(x2, y2, x1, y1);
        else
            lineLow(x1, y1, x2, y2);
    }
    else
    {
        if (y1 > y2)
            lineHigh(x2, y2, x1, y1);
        else
            lineHigh(x1, y1, x2, y2);
    }
}

/*!
 * \brief Draw a point.
 *
 * \param x X-coordinate.
 * \param y Y-coordinate.
 */
void ArduinoGraphics::point(int x, int y)
{
    if (_stroke)
        set(x, y, _strokeR, _strokeG, _strokeB);
}

/*!
 * \brief Draw a rectangle.
 *
 * \param x Start corner x-coordinate.
 * \param y Start corner y-coordinate.
 * \param width Rectangle width.
 * \param height Rectangle height.
 */
void ArduinoGraphics::rect(int x, int y, int width, int height)
{
    if (!_stroke && !_fill)
        return;

    int x1 = x;
    int y1 = y;
    int x2 = x1 + width - 1;
    int y2 = y1 + height - 1;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
        {
            if ((x == x1 || x == x2 || y == y1 || y == y2) && _stroke)
                set(x, y, _strokeR, _strokeG, _strokeB);    // stroke
            else if (_fill)
                set(x, y, _fillR, _fillG, _fillB);          // fill
        }
    }
}

//

/*!
 * \brief Draw text.
 *
 * \param str The text.
 * \param x Cursor start position x-coordinate.
 * \param y Cursor start position y-coordinate.
 */
void ArduinoGraphics::text(const char* str, int x, int y)
{
    if (!_font || !_stroke)
        return;

    while (*str)
    {
        int c = *str++;

        if (c == '\n')
        {
            y += _font->height;
        }
        else if (c == '\r')
        {
            x = 0;
        }
        else if (c == 0xc2 || c == 0xc3)
        {
            // drop
        }
        else
        {
            const uint16_t* b = _font->data[c];

            if (b == nullptr)
                b =  _font->data[0x20];

            if (b)
                bitmap(b, x, y, _font->width, _font->height);

            x += _font->width;
        }
    }
}

/*!
 * \brief Draw text.
 *
 * See also text(const char*, int, int).
 *
 * \param str The text.
 * \param x Cursor start position x-coordinate.
 * \param y Cursor start position y-coordinate.
 */
void ArduinoGraphics::text(const String& str, int x, int y)
{
    text(str.c_str(), x, y);
}

/*!
 * \brief Set the text font.
 *
 * \param which The font instance to use.
 */
void ArduinoGraphics::textFont(const Font* which)
{
    _font = which;
}

//

/*!
 * \brief Get the current font's character width.
 *
 * \return Width in pixels.
 */
int ArduinoGraphics::textFontWidth() const
{
    return (_font ? _font->width : 0);
}

/*!
 * \brief Get the current font's character height.
 *
 * \return Height in pixels.
 */
int ArduinoGraphics::textFontHeight() const
{
    return (_font ? _font->height : 0);
}

//

/*!
 * \brief Set the color of a pixel.
 *
 * \param x X-coordinate.
 * \param y Y-coordinate.
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
void ArduinoGraphics::set(int x, int y, uint32_t color)
{
    set(x, y, colorR(color), colorG(color), colorB(color));
}

//

/*!
 * \brief Clear text buffer and set cursor position.
 *
 * \param x Cursor position x-coordinate.
 * \param y Cursor position y-coordinate.
 */
void ArduinoGraphics::beginText(int x, int y)
{
    _textBuffer = "";

    _textX = x;
    _textY = y;
}

/*!
 * \brief Clear text buffer, set cursor position and text color.
 *
 * \param x Cursor position x-coordinate.
 * \param y Cursor position y-coordinate.
 * \param r Red color value from 0 to 255.
 * \param g Green color value from 0 to 255.
 * \param b Blue color value from 0 to 255.
 */
void ArduinoGraphics::beginText(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    beginText(x, y);

    _textR = r;
    _textG = g;
    _textB = b;
}

/*!
 * \brief Clear text buffer, set cursor position and text color.
 *
 * \param x Cursor position x-coordinate.
 * \param y Cursor position y-coordinate.
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 */
void ArduinoGraphics::beginText(int x, int y, uint32_t color)
{
    beginText(x, y, colorR(color), colorG(color), colorB(color));
}

/*!
 * \brief Draw the text currently in text buffer and clear text buffer.
 *
 * See also text(const String&, int, int).
 */
void ArduinoGraphics::endText()
{
    // backup the stroke color and set the color to the text color
    bool strokeOn = _stroke;
    uint8_t strokeR = _strokeR;
    uint8_t strokeG = _strokeG;
    uint8_t strokeB = _strokeB;

    stroke(_textR, _textG, _textB);

    text(_textBuffer, _textX, _textY);

    // restore the stroke color
    if (strokeOn)
        stroke(strokeR, strokeG, strokeB);
    else
        noStroke();

    // clear the buffer
    _textBuffer = "";
}

//Private

/*!
 * \brief Insert a raw monochrome bitmap.
 *
 * Each \p data element represents a bitmap row and each row is a 16 bit integer
 * and thus represents 16 bitmap columns. Hence \p width can be maximally
 * 16 pixels. The column counting starts from the leftmost bit (MSB).
 * Each set bit results in a pixel with the configured stroke color and
 * each unset bit results in a pixel with the configured background color.
 *
 * \param data Raw data array.
 * \param x Origin x-coordinate.
 * \param y Origin y-coordinate.
 * \param width Bitmap width (maximally 16).
 * \param height Bitmap height.
 */
void ArduinoGraphics::bitmap(const uint16_t* data, int x, int y, int width, int height)
{
    if (!_stroke)
        return;

    if ((data == nullptr) || ((x + width) < 0) || ((y + height) < 0) || (x > _width) || (y > _height))
        return; // offscreen

    for (int j = 0; j < height; j++)
    {
        uint16_t b = data[j];

        for (int i = 0; i < width; i++)
        {
            if (b & (1 << (15 - i)))
                set(x + i, y + j, _strokeR, _strokeG, _strokeB);
            else
                set(x + i, y + j, _backgroundR, _backgroundG, _backgroundB);
        }
    }
}

//

/*!
 * \brief Draw flat oblique lines with angle below 45 degrees to x-axis.
 *
 * \param x1 Start point x-coordinate.
 * \param y1 Start point y-coordinate.
 * \param x2 End point x-coordinate.
 * \param y2 End point y-coordinate.
 */
void ArduinoGraphics::lineLow(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int yi = 1;

    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }

    int D = 2 * dy - dx;
    int y = y1;

    for (int x = x1; x <= x2; x++)
    {
        set(x, y, _strokeR, _strokeG, _strokeB);

        if (D > 0)
        {
            y += yi;
            D -= (2 * dx);
        }

        D += (2 * dy);
    }
}

/*!
 * \brief Draw steep oblique lines with angle at least 45 degrees to x-axis.
 *
 * \param x1 Start point x-coordinate.
 * \param y1 Start point y-coordinate.
 * \param x2 End point x-coordinate.
 * \param y2 End point y-coordinate.
 */
void ArduinoGraphics::lineHigh(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int xi = 1;

    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }

    int D = 2 * dx - dy;
    int x = x1;

    for (int y = y1; y <= y2; y++)
    {
        set(x, y, _strokeR, _strokeG, _strokeB);

        if (D > 0)
        {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

//

/*!
 * \brief Extract red color value from RGB color value.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 * \return 8 bit red color value r[7:0].
 */
uint8_t ArduinoGraphics::colorR(uint32_t color)
{
    return (color >> 16);
}

/*!
 * \brief Extract green color value from RGB color value.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 * \return 8 bit green color value g[7:0].
 */
uint8_t ArduinoGraphics::colorG(uint32_t color)
{
    return (color >> 8);
}

/*!
 * \brief Extract blue color value from RGB color value.
 *
 * \param color RGB color as 32 bit value like {xxxxxxxx,r[7:0],g[7:0],b[7:0]}.
 * \return 8 bit blue color value b[7:0].
 */
uint8_t ArduinoGraphics::colorB(uint32_t color)
{
    return (color >> 0);
}

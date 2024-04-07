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

#ifndef PINS_H
#define PINS_H

#include "usedisplay.h"

#include <Arduino.h>

typedef uint8_t Pin;

/*!
 * \brief Arduino pin definitions.
 *
 * Defines the usage/assignment of all the different Arduino pins.
 */
namespace Pins
{

static constexpr Pin LED_RGB_R = LEDR;      ///< Red LED of on-board RGB LED.
static constexpr Pin LED_RGB_G = LEDG;      ///< GreenLED of on-board RGB LED.
static constexpr Pin LED_RGB_B = LEDB;      ///< Blue LED of on-board RGB LED.
static constexpr Pin LED_Power = LED_PWR;   ///< On-board power LED.
//
static constexpr Pin Buzzer = 9;            ///< Buzzer control.
//
static constexpr Pin PushButtonClr = 10;    ///< Push button for "clear" functionality and run mode RunMode::UnmaskDisturbers.
static constexpr Pin PushButtonDsp = 8;     ///< Push button for "display" functionality and run mode RunMode::TuneAntenna.
//
static constexpr Pin DIPSel1 = 3;           ///< DIP switch configuration "select" group #1.
static constexpr Pin DIPSel2 = 4;           ///< DIP switch configuration "select" group #2.
static constexpr Pin DIPSel3 = 5;           ///< DIP switch configuration "select" group #3.
static constexpr Pin DIPSel4 = 6;           ///< DIP switch configuration "select" group #4.
static constexpr Pin DIPSel5 = 7;           ///< DIP switch configuration "select" group #5.
//
static constexpr Pin DIPSens1 = A0;         ///< DIP switch configuration "sense" group #1.
static constexpr Pin DIPSens2 = A1;         ///< DIP switch configuration "sense" group #2.
static constexpr Pin DIPSens3 = A2;         ///< DIP switch configuration "sense" group #3.
static constexpr Pin DIPSens4 = A3;         ///< DIP switch configuration "sense" group #4.
static constexpr Pin DIPSens5 = A6;         ///< DIP switch configuration "sense" group #5.
//
static constexpr Pin IRQ = 2;               ///< Interrupt request input from AS3935.
static constexpr Pin SPI_CS = A7;           ///< SPI \p chip \p select output for AS3935.
//
static constexpr Pin SPI_BUS_SCK = 13;      ///< SPI bus \p SCK pin.
static constexpr Pin SPI_BUS_MISO = 12;     ///< SPI bus \p MISO pin.
static constexpr Pin SPI_BUS_MOSI = 11;     ///< SPI bus \p MOSI pin.
//
static constexpr Pin I2C_BUS_SCL = A5;      ///< I2C bus \p SCL pin.
static constexpr Pin I2C_BUS_SDA = A4;      ///< I2C bus \p SDA pin.

} // namespace Pins

#if USE_DISPLAY == USE_DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER

/*!
 * \brief Pin definitions of the SX1509 IO expander.
 *
 * Defines the IO numbers of the external pins that are to be provided by the SX1509.
 */
namespace SX1509Pins
{

static constexpr Pin Display_BUSY = 12;     ///< SX1509 pin number to connect display \p BUSY pin to.
static constexpr Pin Display_RST = 13;      ///< SX1509 pin number to connect display \p RST pin to.
static constexpr Pin Display_DC = 14;       ///< SX1509 pin number to connect display \p DC pin to.
static constexpr Pin Display_CS = 15;       ///< SX1509 pin number to connect display \p CS pin to.

} // namespace SX1509Pins

#endif

#endif // PINS_H

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

#ifndef DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER_H
#define DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER_H

#include "abstractspidisplay.h"

#include "as3935.h"
#include "auxil.h"

#include <Arduino.h>

#define DISPLAY_TYPE Display_WaveshareEPaper154BW_SX1509IOExpander

/*!
 * \brief Display driver class for Waveshare 1.54 inch e-paper display with SX1509 I/O expander.
 *
 * Implements the AbstractDisplay interface for the 1.54 inch e-paper display by
 * Waveshare with the additional required I/O pins ('BUSY', 'RST', 'DC', 'CS') being
 * controlled by a SX1509 I/O expander chip that, in turn, is controlled via the I2C bus.
 *
 * The size of 200x200 pixels is large enough for all information that is passed to
 * the update function to be simultaneously shown on the display (see also update()).
 *
 * \attention You must call setup() before using the class.
 */
class DISPLAY_TYPE : public AbstractSPIDisplay
{
public:
    DISPLAY_TYPE(uint8_t pSX1509Addr, uint8_t pPinNumDspCS, uint8_t pPinNumDspDC, uint8_t pPinNumDspRST, uint8_t pPinNumDspBUSY);
                                        ///< Constructor.
    //
    void setup() const override final;  ///< \copybrief AbstractSPIDisplay::setup()
    //
    void init() const override final;   ///< \copybrief AbstractSPIDisplay::init()
    void sleep() const override final;  ///< \copybrief AbstractSPIDisplay::sleep()
    //
    void clear() override final;        ///< \copybrief AbstractSPIDisplay::clear()
    void update(size_t pNumLightnings, float pLightningRate, uint8_t pStormDist, float pBatteryPercentage, float pBatteryVoltage,
                float pRunTimeHours, Auxil::RunMode pMode, bool pSerialEnabled, AS3935::InterruptType pLastInterrupt) override final;
                                        ///< \copybrief AbstractSPIDisplay::update()

private:
    void setPixel(size_t pX, size_t pY, uint8_t pR, uint8_t pG, uint8_t pB) override final; ///< \copybrief AbstractSPIDisplay::setPixel()
    //
    void updateDisplay() const;                 ///< Update the display according to the current buffer content.
    //
    bool displayBusy() const;                   ///< Check if display controller is busy.
    void waitDisplayIdle() const;               ///< Wait until display controller is not busy.
    //
    void hwResetDisplay() const;                ///< Reset display controller via hardware reset pin.
    void turnOffDisplay() const;                ///< Fully turn off the display.
    //
    void selectChip() const override final;     ///< \copybrief AbstractSPIDisplay::selectChip()
    void deselectChip() const override final;   ///< \copybrief AbstractSPIDisplay::deselectChip()
    //
    void setSPIDCCommand() const;               ///< Configure display DC pin for "command" transaction.
    void setSPIDCData() const;                  ///< Configure display DC pin for "data" transaction.
    //
    void writeCommand(uint8_t pCmd) const;                                              ///< Write a display command.
    void writeCommand(uint8_t pCmd, uint8_t pData1) const;                              ///< Write a display command with one data byte.
    void writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2) const;              ///< Write a display command with two data bytes.
    void writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2, uint8_t pData3) const;
                                                                                        ///< Write a display command with three data bytes.
    void writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2, uint8_t pData3, uint8_t pData4) const;
                                                                                        ///< Write a display command with four data bytes.
    void writeCommandRaw(uint8_t pCmd, const uint8_t* pData, uint16_t pLength) const;   ///< Write a display command with many data bytes.

private:
    enum class SX1509RegisterAddress : uint8_t;

private:
    void writeRegSX1509(SX1509RegisterAddress pAddr, uint8_t pVal) const;   ///< Write to SX1509 register.
    uint8_t readRegSX1509(SX1509RegisterAddress pAddr) const;               ///< Read from SX1509 register.
    //
    void setPinsHighSX1509(uint8_t pBankBMask, uint8_t pBankAMask) const;   ///< Set some SX1509 output pins to high state.
    void setPinsLowSX1509(uint8_t pBankBMask, uint8_t pBankAMask) const;    ///< Set some SX1509 output pins to low state.
    //
    static uint16_t sx1509PinToBankMask(uint8_t pPin);  ///< Convert SX1509 pin number to a combined bit mask for both IO banks.
    static uint8_t sx1509PinToBankAMask(uint8_t pPin);  ///< Convert SX1509 pin number to a bit mask for IO bank A.
    static uint8_t sx1509PinToBankBMask(uint8_t pPin);  ///< Convert SX1509 pin number to a bit mask for IO bank B.

private:
    const uint8_t pinNumDspCS;      ///< SX1509 pin for display's CS pin.
    const uint8_t pinNumDspDC;      ///< SX1509 pin for display's DC pin.
    const uint8_t pinNumDspRST;     ///< SX1509 pin for display's RST pin.
    const uint8_t pinNumDspBUSY;    ///< SX1509 pin for display's BUSY pin.
    //
    const uint8_t bankAMaskDspCS;   ///< SX1509 IO bank A bit mask for display's CS pin.
    const uint8_t bankBMaskDspCS;   ///< SX1509 IO bank B bit mask for display's CS pin.
    const uint8_t bankAMaskDspDC;   ///< SX1509 IO bank A bit mask for display's DC pin.
    const uint8_t bankBMaskDspDC;   ///< SX1509 IO bank B bit mask for display's DC pin.
    const uint8_t bankAMaskDspRST;  ///< SX1509 IO bank A bit mask for display's RST pin.
    const uint8_t bankBMaskDspRST;  ///< SX1509 IO bank B bit mask for display's RST pin.
    const uint8_t bankAMaskDspBUSY; ///< SX1509 IO bank A bit mask for display's BUSY pin.
    const uint8_t bankBMaskDspBUSY; ///< SX1509 IO bank B bit mask for display's BUSY pin.
    //
    uint8_t displayBuffer[5000];    ///< Monochrome linear display buffer with one bit per pixel.

private:
    /*!
     * \brief SX1509 register addresses.
     */
    enum class SX1509RegisterAddress : uint8_t
    {
        RegInputDisableB = 0x00,    ///< Disable input buffer bit mask [IO bank B].
        RegInputDisableA = 0x01,    ///< Disable input buffer bit mask [IO bank A].
        RegPullUpB = 0x06,          ///< Enable pull-up resistor bit mask [IO bank B].
        RegPullUpA = 0x07,          ///< Enable pull-up resistor bit mask [IO bank A].
        RegPullDownB = 0x08,        ///< Enable pull-down resistor bit mask [IO bank B].
        RegPullDownA = 0x09,        ///< Enable pull-down resistor bit mask [IO bank A].
        RegDirB = 0x0E,             ///< Configure as input bit mask [IO bank B].
        RegDirA = 0x0F,             ///< Configure as input bit mask [IO bank A].
        RegDataB = 0x10,            ///< Set/read IO state [IO bank B].
        RegDataA = 0x11,            ///< Set/read IO state [IO bank A].
        RegReset = 0x7D             ///< Software reset.
    };
};

#endif // DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER_H

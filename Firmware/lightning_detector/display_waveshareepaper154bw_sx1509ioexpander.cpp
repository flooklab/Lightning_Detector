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

#include "display_waveshareepaper154bw_sx1509ioexpander.h"

#include "fonts.h"
#include "spiconfig.h"

#include <SPI.h>
#include <Wire.h>

/*!
 * \brief Constructor.
 *
 * Configures the Graphics instance to work with this display driver instance and
 * sets background and stroke colors to obtain black strokes on a white background.
 * See also setPixel().
 *
 * \param pSX1509Addr I2C address of the SX1509 IO expander chip.
 * \param pPinNumDspCS SX1509 pin number to connect display's \p CS pin to.
 * \param pPinNumDspDC SX1509 pin number to connect display's \p DC pin to.
 * \param pPinNumDspRST SX1509 pin number to connect display's \p RST pin to.
 * \param pPinNumDspBUSY SX1509 pin number to connect display's \p BUSY pin to.
 */
DISPLAY_TYPE::DISPLAY_TYPE(uint8_t pSX1509Addr, uint8_t pPinNumDspCS, uint8_t pPinNumDspDC, uint8_t pPinNumDspRST, uint8_t pPinNumDspBUSY) :
    AbstractSPIDisplay(200, 200, SPISettings(SPIConfig::getSPISpeed(), MSBFIRST, SPI_MODE0), pSX1509Addr),
    pinNumDspCS(pPinNumDspCS),
    pinNumDspDC(pPinNumDspDC),
    pinNumDspRST(pPinNumDspRST),
    pinNumDspBUSY(pPinNumDspBUSY),
    bankAMaskDspCS(sx1509PinToBankAMask(pinNumDspCS)),
    bankBMaskDspCS(sx1509PinToBankBMask(pinNumDspCS)),
    bankAMaskDspDC(sx1509PinToBankAMask(pinNumDspDC)),
    bankBMaskDspDC(sx1509PinToBankBMask(pinNumDspDC)),
    bankAMaskDspRST(sx1509PinToBankAMask(pinNumDspRST)),
    bankBMaskDspRST(sx1509PinToBankBMask(pinNumDspRST)),
    bankAMaskDspBUSY(sx1509PinToBankAMask(pinNumDspBUSY)),
    bankBMaskDspBUSY(sx1509PinToBankBMask(pinNumDspBUSY))
{
    graphics.setDisplay(this);

    graphics.begin();
    graphics.background(1, 1, 1);
    graphics.stroke(0, 0, 0);
}

//Public

/*!
 * \copybrief AbstractSPIDisplay::setup()
 *
 * Enables SPI and I2C.
 *
 * Resets SX1509 and configures its IOs:
 * - \p CS, \p DC and \p RST as outputs; inputs otherwise
 * - Set high level for active-low \p CS and \p RST
 * - Enable pull-down and disable input buffer for every unused pin
 *
 * Finally issues an initial display hardware reset.
 */
void DISPLAY_TYPE::setup() const
{
    //Enable SPI and I2C
    AbstractSPIDisplay::setup();

    //Reset SX1509 and configure its IOs

    //Reset sequence
    writeRegSX1509(SX1509RegisterAddress::RegReset, 0x12);
    writeRegSX1509(SX1509RegisterAddress::RegReset, 0x34);

    //Only CS, DC and RST are outputs
    writeRegSX1509(SX1509RegisterAddress::RegDirB, ~(bankBMaskDspCS | bankBMaskDspDC | bankBMaskDspRST));
    writeRegSX1509(SX1509RegisterAddress::RegDirA, ~(bankAMaskDspCS | bankAMaskDspDC | bankAMaskDspRST));

    //Start with high output for active-low CS and RST
    writeRegSX1509(SX1509RegisterAddress::RegDataB, (bankBMaskDspCS | bankBMaskDspRST));
    writeRegSX1509(SX1509RegisterAddress::RegDataA, (bankAMaskDspCS | bankAMaskDspRST));

    //Use pull-down on every unused pin
    writeRegSX1509(SX1509RegisterAddress::RegPullUpB, 0b0);
    writeRegSX1509(SX1509RegisterAddress::RegPullUpA, 0b0);
    writeRegSX1509(SX1509RegisterAddress::RegPullDownB, ~(bankBMaskDspCS | bankBMaskDspDC | bankBMaskDspRST | bankBMaskDspBUSY));
    writeRegSX1509(SX1509RegisterAddress::RegPullDownA, ~(bankAMaskDspCS | bankAMaskDspDC | bankAMaskDspRST | bankAMaskDspBUSY));

    //Disable input buffers of unused pins
    writeRegSX1509(SX1509RegisterAddress::RegInputDisableB, ~(bankBMaskDspCS | bankBMaskDspDC | bankBMaskDspRST | bankBMaskDspBUSY));
    writeRegSX1509(SX1509RegisterAddress::RegInputDisableA, ~(bankBMaskDspCS | bankBMaskDspDC | bankBMaskDspRST | bankBMaskDspBUSY));

    //Make sure again that CS is high
    deselectChip();

    //Initial display hardware reset
    hwResetDisplay();
}

//

/*!
 * \copybrief AbstractSPIDisplay::init()
 *
 * Display hardware reset plus following init sequence:
 * - \p 0x12
 * - \p 0x01, \p 0XC7, \p 0x00, \p 0x00
 * - \p 0x11, \p 0x03
 * - \p 0x44, \p 0x00, \p 0x18
 * - \p 0x45, \p 0x00, \p 0x00, \p 0xC7, \p 0x00
 */
void DISPLAY_TYPE::init() const
{
    hwResetDisplay();

    writeCommand(0x12);

    waitDisplayIdle();

    writeCommand(0x01, 0XC7, 0x00, 0x00);

    writeCommand(0x11, 0x03);   //Incrementing X, incrementing Y, auto update address along x-axis

    writeCommand(0x44, 0x00, 0x18);
    writeCommand(0x45, 0x00, 0x00, 0xC7, 0x00);
}

/*!
 * \copybrief AbstractSPIDisplay::sleep()
 *
 * Sends sleep command and then fully turns off display power via turnOffDisplay().
 */
void DISPLAY_TYPE::sleep() const
{
    waitDisplayIdle();

    writeCommand(0x10, 0x01);

    turnOffDisplay();
}

//

/*!
 * \copybrief AbstractSPIDisplay::clear()
 *
 * Resets the buffer to all white and calls updateDisplay().
 */
void DISPLAY_TYPE::clear()
{
    for (size_t i = 0; i < 5000; ++i)
        displayBuffer[i] = 0b11111111;

    updateDisplay();
}

//

/*!
 * \copybrief AbstractSPIDisplay::update()
 *
 * Shows all of the passed information on the display.
 * The buffer is written and then updateDisplay() is called to write the buffer out.
 *
 * The displayed information is formatted in the following way (~ from top-left to bottom-right):
 * - \p pNumLightnings
 * - \p pLightningRate
 * - Battery state as both \p pBatteryVoltage and \p pBatteryPercentage
 * - \p pStormDist ("???" if AS3935 reports out of range)
 * - Short status information (from left to right):
 *   - 'D' for \p pMode = Auxil::RunMode::UnmaskDisturbers, 'T' for Auxil::RunMode::TuneAntenna, ' ' (space) else
 *   - 'S' if \p pSerialEnabled and ' ' (space) else
 *   - \p pLastInterrupt encoded as follows:
 *     - "" for AS3935::InterruptType::Lightning
 *     - "dDst" for AS3935::InterruptType::DistanceChanged
 *     - "Dist" for AS3935::InterruptType::Disturber
 *     - "NOIS" for AS3935::InterruptType::Noise
 *     - "INV" for AS3935::InterruptType::Invalid
 * - \p pRunTimeHours as raw hours if below 24h or as days+hours otherwise.
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
    //Prepare formatted strings

    auto rightAlignString = [](String& pStr, size_t pLength) -> void
    {
        if (pLength < pStr.length())
            return;

        size_t diff = pLength - pStr.length();

        for (size_t i = 0; i < diff; ++i)
            pStr = String(" ") + pStr;
    };

    size_t numLghtStrMaxLen = 10;
    String numLghtStr(pNumLightnings);
    if (numLghtStr.length() > numLghtStrMaxLen)
        numLghtStr = "NaN";

    size_t rateStrMaxLen = 5;
    size_t rateStrDecPlaces = 0;
    if (static_cast<size_t>(pLightningRate) < 10)
        rateStrDecPlaces = 1;
    String rateStr(pLightningRate, rateStrDecPlaces);
    rateStr += "/m";
    if (rateStr.length() > rateStrMaxLen)
        rateStr = "NaN";
    rightAlignString(rateStr, rateStrMaxLen);

    size_t voltageStrMaxLen = 7;
    String voltageStr(pBatteryVoltage, 3);
    voltageStr += " V";
    if (voltageStr.length() > voltageStrMaxLen)
        voltageStr = "NaN";

    size_t distStrMaxLen = 8;
    String distStr;

    if (pStormDist == AS3935::stormDistanceOutOfRange)
        distStr = "???";
    else
        distStr = String(pStormDist);

    distStr += " km";
    if (distStr.length() > distStrMaxLen)
        distStr = "NaN";
    rightAlignString(distStr, distStrMaxLen);

    size_t batFracStrMaxLen = 7;
    String batFracStr(static_cast<size_t>(pBatteryPercentage));
    batFracStr += " %";
    if (batFracStr.length() > batFracStrMaxLen)
        batFracStr = "NaN";

    size_t statusStrMaxLen = 7;
    String statusStr;

    if (pMode == Auxil::RunMode::Normal)
        statusStr += " ";
    else if (pMode == Auxil::RunMode::UnmaskDisturbers)
        statusStr += "D";
    else if (pMode == Auxil::RunMode::TuneAntenna)
        statusStr += "T";
    else
        statusStr += "?";

    if (pSerialEnabled)
        statusStr += "S";
    else
        statusStr += " ";

    statusStr += " ";

    if (pLastInterrupt == AS3935::InterruptType::DistanceChanged)
        statusStr += "dDst";
    else if (pLastInterrupt == AS3935::InterruptType::Noise)
        statusStr += "NOIS";
    else if (pLastInterrupt == AS3935::InterruptType::Disturber)
        statusStr += "Dist";
    else if (pLastInterrupt == AS3935::InterruptType::Lightning)
        statusStr += "";
    else if (pLastInterrupt == AS3935::InterruptType::Invalid)
        statusStr += "INV";
    else
        statusStr += "?";

    if (statusStr.length() > statusStrMaxLen)
        statusStr = "ERROR";

    size_t pRunTimeFullDays = static_cast<size_t>(pRunTimeHours) / 24;
    float pRunTimeRemainderHours = pRunTimeHours - pRunTimeFullDays*24;

    size_t runtimeStrMaxLen = 8;
    String runtimeStr = String(pRunTimeFullDays) + "d ";

    if (pRunTimeFullDays == 0)
        runtimeStr = String(pRunTimeRemainderHours, 1) + " h";
    else
        runtimeStr += String(static_cast<size_t>(pRunTimeRemainderHours)) + "h";

    if (runtimeStr.length() > runtimeStrMaxLen)
        runtimeStr = "NaN";
    rightAlignString(runtimeStr, runtimeStrMaxLen);

    //Clear buffer
    for (size_t i = 0; i < 5000; ++i)
        displayBuffer[i] = 0b11111111;

    //Draw Frame
    const size_t frameMargin = 1;
    graphics.rect(frameMargin, frameMargin, graphics.width() - 2*frameMargin, graphics.height() - 2*frameMargin);

    //Write header/caption text

    graphics.textFont(&Font_TerminASCII24Bold);

    const size_t textHeight = graphics.textFontHeight();
    const size_t textWidth = graphics.textFontWidth();

    const size_t textOfsX = 4;
    const size_t textOfsY = 4;
    const size_t textPadding = 3;
    const size_t captionTextExtraPadding = 3;

    size_t yPos = textOfsY+captionTextExtraPadding;

    graphics.text(" Lightning Det. ", textOfsX, yPos);

    //Write text containing the interesting information

    graphics.textFont(&Font_TerminASCII24);

    yPos += textHeight+captionTextExtraPadding+textPadding;
    graphics.text("Lightnings  Rate", textOfsX, yPos);
    yPos += textHeight+textPadding;
    graphics.text(numLghtStr, textOfsX, yPos);
    graphics.text(rateStr, textOfsX + 11*textWidth, yPos);
    yPos += textHeight+textPadding;
    graphics.text("Battery Distance", textOfsX, yPos);
    yPos += textHeight+textPadding;
    graphics.text(voltageStr, textOfsX, yPos);
    graphics.text(distStr, textOfsX + 8*textWidth, yPos);
    yPos += textHeight+textPadding;
    graphics.text(batFracStr, textOfsX, yPos);
    graphics.text("Run Time", textOfsX + 8*textWidth, yPos);
    yPos += textHeight+textPadding;
    graphics.text(statusStr, textOfsX, yPos);
    graphics.text(runtimeStr, textOfsX + 8*textWidth, yPos);

    //Add decoration lines

    const size_t yPos0 = textOfsY+captionTextExtraPadding+textHeight+captionTextExtraPadding+1;
    yPos = yPos0;

    graphics.line(frameMargin, yPos, graphics.width()-1-frameMargin, yPos);
    yPos += textHeight+textPadding;
    yPos += textHeight+textPadding;
    graphics.line(frameMargin, yPos, graphics.width()-1-frameMargin, yPos);
    graphics.line(textOfsX + static_cast<size_t>(10.5*textWidth), yPos0, textOfsX + static_cast<size_t>(10.5*textWidth), yPos);
    graphics.line(textOfsX + static_cast<size_t>(7.5*textWidth), yPos, textOfsX + static_cast<size_t>(7.5*textWidth),
                  graphics.height()-1-frameMargin);
    yPos += textHeight+textPadding;
    yPos += textHeight+textPadding;
    graphics.line(textOfsX + static_cast<size_t>(7.5*textWidth), yPos, graphics.width()-1-frameMargin, yPos);
    yPos += textHeight+textPadding;
    graphics.line(frameMargin, yPos, textOfsX + static_cast<size_t>(7.5*textWidth), yPos);

    updateDisplay();
}

//Private

/*!
 * \copybrief AbstractSPIDisplay::setPixel()
 *
 * Sets the pixel value for pixel (\p pX, \p pY), ignoring the specified color value (monochrome display).
 * If any of \p pR, \p pG or \p pB are larger than zero the pixel value is set to 1 and it is set to zero otherwise.
 *
 * \param pX Display's x-coordinate.
 * \param pY Display's y-coordinate.
 * \param pR Red pixel value from 0 to 255.
 * \param pG Green pixel value from 0 to 255.
 * \param pB Blue pixel value from 0 to 255.
 */
void DISPLAY_TYPE::setPixel(size_t pX, size_t pY, uint8_t pR, uint8_t pG, uint8_t pB)
{
    size_t pixIdx = graphics.width()*pY + pX;

    size_t byteIdx = pixIdx / 8;
    size_t bitIdx = pixIdx % 8;

    uint8_t byteMask = 0b10000000 >> bitIdx;

    bool set = ((pR > 0) || (pG > 0) || (pB > 0));

    if (set)
        displayBuffer[byteIdx] |= byteMask;
    else
        displayBuffer[byteIdx] &= ~byteMask;
}

//

/*!
 * \brief Update the display according to the current buffer content.
 *
 * Does a full display update sequence using the current state of the bitmap buffer.
 */
void DISPLAY_TYPE::updateDisplay() const
{
    waitDisplayIdle();

    uint16_t tval12bit = 0x140;  //TODO: This is ~20 degrees; how to use internal sensor???
    writeCommand(0x1A, (tval12bit >> 4), ((tval12bit & 0b000000001111) << 4));

    writeCommand(0x22, 0xB1);

    writeCommand(0x20);

    waitDisplayIdle();

    writeCommand(0x4E, 0x00);
    writeCommand(0x4F, 0X00, 0x00);

    writeCommandRaw(0x24, &displayBuffer[0], 5000);

    writeCommand(0x22, 0xC7);

    writeCommand(0x20);

    waitDisplayIdle();
}

//

/*!
 * \brief Check if display controller is busy.
 *
 * \return True if \p BUSY pin reports being busy and false otherwise.
 */
bool DISPLAY_TYPE::displayBusy() const
{
    bool inputHighB = (readRegSX1509(SX1509RegisterAddress::RegDataB) & bankBMaskDspBUSY) != 0b0;
    bool inputHighA = (readRegSX1509(SX1509RegisterAddress::RegDataA) & bankAMaskDspBUSY) != 0b0;

    return inputHighB || inputHighA;
}

/*!
 * \brief Wait until display controller is not busy.
 *
 * See also displayBusy().
 */
void DISPLAY_TYPE::waitDisplayIdle() const
{
    while (displayBusy())
        delay(10);
}

//

/*!
 * \brief Reset display controller via hardware reset pin.
 */
void DISPLAY_TYPE::hwResetDisplay() const
{
    setPinsHighSX1509(bankBMaskDspRST, bankAMaskDspRST);    //First set RST high, as constant low RST eventually turns off power
    delay(100);
    setPinsLowSX1509(bankBMaskDspRST, bankAMaskDspRST);
    delay(10);
    setPinsHighSX1509(bankBMaskDspRST, bankAMaskDspRST);
    delay(10);
}

/*!
 * \brief Fully turn off the display.
 *
 * Sets \p RST pin to low, which eventually makes the display controller to turn off power by itself.
 */
void DISPLAY_TYPE::turnOffDisplay() const
{
    //Constant high RST draws ~2mA current, so turn off power completely, which is eventually triggered by constant low RST
    setPinsLowSX1509(bankBMaskDspRST, bankAMaskDspRST);
}

//

/*!
 * \copybrief AbstractSPIDisplay::selectChip()
 */
void DISPLAY_TYPE::selectChip() const
{
    setPinsLowSX1509(bankBMaskDspCS, bankAMaskDspCS);
}

/*!
 * \copybrief AbstractSPIDisplay::deselectChip()
 */
void DISPLAY_TYPE::deselectChip() const
{
    setPinsHighSX1509(bankBMaskDspCS, bankAMaskDspCS);
}

//

/*!
 * \brief Configure display DC pin for "command" transaction.
 */
void DISPLAY_TYPE::setSPIDCCommand() const
{
    setPinsLowSX1509(bankBMaskDspDC, bankAMaskDspDC);
}

/*!
 * \brief Configure display DC pin for "data" transaction.
 */
void DISPLAY_TYPE::setSPIDCData() const
{
    setPinsHighSX1509(bankBMaskDspDC, bankAMaskDspDC);
}

//

/*!
 * \brief Write a display command.
 *
 * \param pCmd Command byte.
 */
void DISPLAY_TYPE::writeCommand(uint8_t pCmd) const
{
    writeCommandRaw(pCmd, nullptr, 0);
}

/*!
 * \brief Write a display command with one data byte.
 *
 * \param pCmd Command byte.
 * \param pData1 Data byte 1.
 */
void DISPLAY_TYPE::writeCommand(uint8_t pCmd, uint8_t pData1) const
{
    uint8_t data[1] = {pData1};
    writeCommandRaw(pCmd, &data[0], 1);
}

/*!
 * \brief Write a display command with two data bytes.
 *
 * Order: writes \p pCmd, then \p pData1, then \p pData2.
 *
 * \param pCmd Command byte.
 * \param pData1 Data byte 1.
 * \param pData2 Data byte 2.
 */
void DISPLAY_TYPE::writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2) const
{
    uint8_t data[2] = {pData1, pData2};
    writeCommandRaw(pCmd, &data[0], 2);
}

/*!
 * \brief Write a display command with three data bytes.
 *
 * Order: writes \p pCmd, then \p pData1, then \p pData2, etc.
 *
 * \param pCmd Command byte.
 * \param pData1 Data byte 1.
 * \param pData2 Data byte 2.
 * \param pData3 Data byte 3.
 */
void DISPLAY_TYPE::writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2, uint8_t pData3) const
{
    uint8_t data[3] = {pData1, pData2, pData3};
    writeCommandRaw(pCmd, &data[0], 3);
}

/*!
 * \brief Write a display command with four data bytes.
 *
 * Order: writes \p pCmd, then \p pData1, then \p pData2, etc.
 *
 * \param pCmd Command byte.
 * \param pData1 Data byte 1.
 * \param pData2 Data byte 2.
 * \param pData3 Data byte 3.
 * \param pData4 Data byte 4.
 */
void DISPLAY_TYPE::writeCommand(uint8_t pCmd, uint8_t pData1, uint8_t pData2, uint8_t pData3, uint8_t pData4) const
{
    uint8_t data[4] = {pData1, pData2, pData3, pData4};
    writeCommandRaw(pCmd, &data[0], 4);
}

/*!
 * \brief Write a display command with many data bytes.
 *
 * Order: writes \p pCmd, then \p pData[0], then \p pData[1], etc.
 *
 * \param pCmd Command byte.
 * \param pData Data bytes.
 * \param pLength Number of data bytes.
 */
void DISPLAY_TYPE::writeCommandRaw(uint8_t pCmd, const uint8_t* pData, uint16_t pLength) const
{
    setSPIDCCommand();

    SPI.beginTransaction(spiSettings);
    selectChip();

    delayMicroseconds(20);

    SPI.transfer(pCmd);

    if (pLength > 0)
    {
        setSPIDCData();
        delayMicroseconds(20);
    }

    for (uint16_t i = 0; i < pLength; ++i)
        SPI.transfer(*(pData+i));

    deselectChip();
    SPI.endTransaction();

    setSPIDCCommand();
}

//

/*!
 * \brief Write to SX1509 register.
 *
 * \param pAddr Register address.
 * \param pVal New value.
 */
void DISPLAY_TYPE::writeRegSX1509(SX1509RegisterAddress pAddr, uint8_t pVal) const
{
    uint8_t regAddr = static_cast<uint8_t>(pAddr);

    Wire.beginTransmission(ioExpanderI2CAddr);

    Wire.write(regAddr);
    Wire.write(pVal);

    Wire.endTransmission(true);
}

/*!
 * \brief Read from SX1509 register.
 *
 * \param pAddr Register address.
 * \return Read value.
 */
uint8_t DISPLAY_TYPE::readRegSX1509(SX1509RegisterAddress pAddr) const
{
    uint8_t regAddr = static_cast<uint8_t>(pAddr);

    Wire.beginTransmission(ioExpanderI2CAddr);

    Wire.write(regAddr);

    Wire.endTransmission(false);

    Wire.requestFrom(ioExpanderI2CAddr, 1, true);

    uint8_t retVal = Wire.read();

    return retVal;
}

//

/*!
 * \brief Set some SX1509 output pins to high state.
 *
 * Output pins corresponding to a '1' bit in the bit masks \p pBankBMask and
 * \p pBankAMask are set to \e high state while keeping the state of all other pins.
 *
 * \param pBankBMask Bit mask for SX1509's IO bank B.
 * \param pBankAMask Bit mask for SX1509's IO bank A.
 */
void DISPLAY_TYPE::setPinsHighSX1509(uint8_t pBankBMask, uint8_t pBankAMask) const
{
    uint8_t dataB = readRegSX1509(SX1509RegisterAddress::RegDataB);
    uint8_t dataA = readRegSX1509(SX1509RegisterAddress::RegDataA);

    dataB |= pBankBMask;
    dataA |= pBankAMask;

    writeRegSX1509(SX1509RegisterAddress::RegDataB, dataB);
    writeRegSX1509(SX1509RegisterAddress::RegDataA, dataA);
}

/*!
 * \brief Set some SX1509 output pins to low state.
 *
 * Output pins corresponding to a '1' bit in the bit masks \p pBankBMask and
 * \p pBankAMask are set to \e low state while keeping the state of all other pins.
 *
 * \param pBankBMask Bit mask for SX1509's IO bank B.
 * \param pBankAMask Bit mask for SX1509's IO bank A.
 */
void DISPLAY_TYPE::setPinsLowSX1509(uint8_t pBankBMask, uint8_t pBankAMask) const
{
    uint8_t dataB = readRegSX1509(SX1509RegisterAddress::RegDataB);
    uint8_t dataA = readRegSX1509(SX1509RegisterAddress::RegDataA);

    dataB &= ~pBankBMask;
    dataA &= ~pBankAMask;

    writeRegSX1509(SX1509RegisterAddress::RegDataB, dataB);
    writeRegSX1509(SX1509RegisterAddress::RegDataA, dataA);
}

//

/*!
 * \brief Convert SX1509 pin number to a combined bit mask for both IO banks.
 *
 * Generates a bit pattern {\p p15, ..., \p p0} with all zeroes except a one at \p pi = \p pPin.
 *
 * \param pPin SX1509 pin number.
 * \return 16 bit long bit mask with 1 for \p pPin and 0 else.
 */
uint16_t DISPLAY_TYPE::sx1509PinToBankMask(uint8_t pPin)
{
    uint16_t pinMask = (0b1 << pPin);

    return pinMask;
}

/*!
 * \brief Convert SX1509 pin number to a bit mask for IO bank A.
 *
 * Generates a bit pattern {\p p7, ..., \p p0} with all zeroes except a one at \p pi = \p pPin.
 *
 * \param pPin SX1509 pin number.
 * \return 8 bit long bit mask (just bank A) with 1 for \p pPin and 0 else.
 */
uint8_t DISPLAY_TYPE::sx1509PinToBankAMask(uint8_t pPin)
{
    return static_cast<uint8_t>(sx1509PinToBankMask(pPin) & 0b0000000011111111);
}

/*!
 * \brief Convert SX1509 pin number to a bit mask for IO bank B.
 *
 * Generates a bit pattern {\p p15, ..., \p p8} with all zeroes except a one at \p pi = \p pPin.
 *
 * \param pPin SX1509 pin number.
 * \return 8 bit long bit mask (just bank B) with 1 for \p pPin and 0 else.
 */
uint8_t DISPLAY_TYPE::sx1509PinToBankBMask(uint8_t pPin)
{
    return static_cast<uint8_t>((sx1509PinToBankMask(pPin) >> 8) & 0b0000000011111111);
}

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

#include "as3935.h"

#include "spiconfig.h"

constexpr uint8_t AS3935::registers[];
constexpr uint8_t AS3935::writableRegisters[];

//

/*!
 * \brief Constructor.
 *
 * \param pSPIChipSelectPin SPI 'chip select' pin for the %AS3935.
 * \param pIRQPin %AS3935 interrupt request pin.
 */
AS3935::AS3935(Pin pSPIChipSelectPin, Pin pIRQPin) :
    spiChipSelectPin(pSPIChipSelectPin),
    irqPin(pIRQPin),
    spiSettings(SPISettings(SPIConfig::getSPISpeed(), MSBFIRST, SPI_MODE1))
{
}

//Public

/*!
 * \brief Configure required pins and buses and the device itself.
 *
 * Sets 'chip select' pin to output mode and not selected.
 * Sets %AS3935 IRQ pin to input mode with pull-down enabled.
 * Enables the SPI bus.
 */
void AS3935::setup() const
{
    pinMode(spiChipSelectPin, OUTPUT);
    deselectChip();

    pinMode(irqPin, INPUT_PULLDOWN);

    SPIConfig::enableSPI();
}

//

/*!
 * \brief Attach interrupt request pin as Arduino interrupt.
 *
 * \param pCallback ISR function to call upon detected interrupt.
 */
void AS3935::enableInterrupt(ISRCallbackPtr pCallback) const
{
    attachInterrupt(digitalPinToInterrupt(irqPin), pCallback, RISING);
    pinMode(irqPin, INPUT_PULLDOWN);    //Fixes pull-down configuration, which is somehow changed to pull-up by attachInterrupt()
}

/*!
 * \brief Detach Arduino interrupt for interrupt request pin.
 */
void AS3935::disableInterrupt() const
{
    detachInterrupt(digitalPinToInterrupt(irqPin));
}

//

/*!
 * \brief Write configuration registers.
 *
 * \param pConfig New configuration to write.
 */
void AS3935::writeConfiguration(const Configuration& pConfig) const
{
    uint8_t afeGb = pConfig.getRegister(Configuration::RegIdent::AFE_GB);
    writeReg(0x00, (afeGb << 1));

    uint8_t nfLev = pConfig.getRegister(Configuration::RegIdent::NF_LEV);
    uint8_t wdTh = pConfig.getRegister(Configuration::RegIdent::WDTH);
    writeReg(0x01, ((nfLev << 4) | wdTh));

    uint8_t minNumLigh = pConfig.getRegister(Configuration::RegIdent::MIN_NUM_LIGH);
    uint8_t sRej = pConfig.getRegister(Configuration::RegIdent::SREJ);
    writeReg(0x02, ((((((static_cast<uint8_t>(1) << 1) | static_cast<uint8_t>(1)) << 2) | minNumLigh) << 4) | sRej));

    uint8_t lcoFDiv = pConfig.getRegister(Configuration::RegIdent::LCO_FDIV);
    writeReg(0x03, ((lcoFDiv << 6) | (readReg(0x03) & 0b00100000)));

    uint8_t tunCap = pConfig.getRegister(Configuration::RegIdent::TUN_CAP);
    writeReg(0x08, (tunCap | (readReg(0x08) & 0b10000000)));
}

//

/*!
 * \brief Enable interrupt masking for disturber signals.
 *
 * Disables interrupt requests for recognized disturber signals.
 */
void AS3935::maskDisturbers() const
{
    writeReg(0x03, (readReg(0x03) | 0b00100000));
}

/*!
 * \brief Disable interrupt masking for disturber signals.
 *
 * Enables interrupt requests for recognized disturber signals (%AS3935 POR default).
 */
void AS3935::unmaskDisturbers() const
{
    writeReg(0x03, (readReg(0x03) & 0b11011111));
}

/*!
 * \brief Assign antenna tuning signal to interrupt pin.
 *
 * The %AS3935 chip is configured to generate an antenna resonance
 * signal and copy a digitized version of it to the IRQ pin.
 */
void AS3935::enableAntennaTuning() const
{
    writeReg(0x08, ((readReg(0x08) | 0b10000000) & 0b10011111));
}

/*!
 * \brief Revert assignment of antenna tuning signal to interrupt pin.
 */
void AS3935::disableAntennaTuning() const
{
    writeReg(0x08, (readReg(0x08) & 0b00011111));
}

//

/*!
 * \brief Clear distance estimation statistics.
 */
void AS3935::clearStatistics() const
{
    uint8_t val0 = readReg(0x02) | 0b10000000;

    writeReg(0x02, val0 | 0b01000000);
    writeReg(0x02, val0 & 0b10111111);
    writeReg(0x02, val0 | 0b01000000);
}

//

/*!
 * \brief Check for interrupt request signal.
 *
 * \return True if IRQ is high, false else.
 */
bool AS3935::irqHigh() const
{
    return digitalRead(irqPin) == HIGH;
}

//

/*!
 * \brief Update values from chip according to interrupt type.
 *
 * Reads and returns the interrupt type after an interrupt request.
 *
 * \p pEnergy and \p pDistance are updated for certain interrupt types, see documentation for those parameters.
 *
 * \param pEnergy Set to "lightning energy" (a.u.) in case of InterruptType::Lightning, left unchanged otherwise.
 * \param pDistance Set to current estimated storm distance in case of InterruptType::Lightning and InterruptType::DistanceChanged,
 *                  left unchanged otherwise.
 * \return The reported interrupt type.
 */
AS3935::InterruptType AS3935::processIRQ(uint32_t& pEnergy, uint8_t& pDistance) const
{
    while (digitalRead(irqPin) == LOW)
        ;

    //Must wait at least 2ms before reading interrupt register from AS3935 according to datasheet
    delay(5);

    uint8_t intType = readReg(0x03) & 0b00001111;

    while (digitalRead(irqPin) == HIGH)
        ;

    switch (intType)
    {
        case static_cast<uint8_t>(InterruptType::DistanceChanged):
        {
            pDistance = readReg(0x07) & 0b00111111;
            break;
        }
        case static_cast<uint8_t>(InterruptType::Noise):
        {
            break;
        }
        case static_cast<uint8_t>(InterruptType::Disturber):
        {
            break;
        }
        case static_cast<uint8_t>(InterruptType::Lightning):
        {
            uint8_t energyLS = readReg(0x04);
            uint8_t energyMS = readReg(0x05);
            uint8_t energyMMS = readReg(0x06) & 0b00011111;

            pEnergy = ((((energyMMS << 8) | energyMS) << 8) | energyLS);

            pDistance = readReg(0x07) & 0b00111111;

            break;
        }
        default:
        {
            return InterruptType::Invalid;
            break;
        }
    }

    return static_cast<InterruptType>(intType);
}

//Private

/*!
 * \brief Write value to a register.
 *
 * \param pAddr Register address.
 * \param pVal New value.
 */
void AS3935::writeReg(uint8_t pAddr, uint8_t pVal) const
{
    if (!Auxil::arrayContains<decltype(writableRegisters)>(writableRegisters, pAddr))
        return;

    uint8_t cmdLeft = (0b00000000 | (pAddr & 0b00111111));
    uint8_t cmdRight = pVal;

    SPI.beginTransaction(spiSettings);
    selectChip();

    delayMicroseconds(20);

    SPI.transfer16((static_cast<uint16_t>(cmdLeft) << 8) | static_cast<uint16_t>(cmdRight));

    deselectChip();
    SPI.endTransaction();
}

/*!
 * \brief Read value from a register.
 *
 * \param pAddr Register address.
 * \return Read value.
 */
uint8_t AS3935::readReg(uint8_t pAddr) const
{
    if (!Auxil::arrayContains<decltype(registers)>(registers, pAddr))
        return 0;

    uint8_t cmdLeft = (0b01000000 | (pAddr & 0b00111111));
    uint8_t cmdRight = 0b00000000;

    SPI.beginTransaction(spiSettings);
    selectChip();

    delayMicroseconds(20);

    uint16_t val = SPI.transfer16((static_cast<uint16_t>(cmdLeft) << 8) | static_cast<uint16_t>(cmdRight));

    deselectChip();
    SPI.endTransaction();

    return static_cast<uint8_t>(val & 0b0000000011111111);
}

//

/*!
 * \brief Enable device's SPI 'chip select' signal.
 */
void AS3935::selectChip() const
{
    digitalWrite(spiChipSelectPin, LOW);
}

/*!
 * \brief Disable device's SPI 'chip select' signal.
 */
void AS3935::deselectChip() const
{
    digitalWrite(spiChipSelectPin, HIGH);
}

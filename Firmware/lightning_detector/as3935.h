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

#ifndef AS3935_H
#define AS3935_H

#include "auxil.h"
#include "configuration.h"
#include "pins.h"

#include <Arduino.h>
#include <SPI.h>

/*!
 * \brief Driver class for the %AS3935 lightning sensor.
 *
 * Provides configuration handling and interrupt handling and processing for the %AS3935 chip.
 *
 * \attention You must call setup() before using the class.
 */
class AS3935
{
public:
    enum class InterruptType : uint8_t;

public:
    using ISRCallbackPtr = Auxil::ISRCallbackPtr;   ///< \copybrief Auxil::ISRCallbackPtr

public:
    AS3935(Pin pSPIChipSelectPin, Pin pIRQPin);     ///< Constructor.
    //
    void setup() const;                             ///< Configure required pins and buses and the device itself.
    //
    void enableInterrupt(ISRCallbackPtr pCallback) const;   ///< Attach interrupt request pin as Arduino interrupt.
    void disableInterrupt() const;                          ///< Detach Arduino interrupt for interrupt request pin.
    //
    void writeConfiguration(const Configuration& pConfig) const;    ///< Write configuration registers.
    //
    void maskDisturbers() const;        ///< Enable interrupt masking for disturber signals.
    void unmaskDisturbers() const;      ///< Disable interrupt masking for disturber signals.
    void enableAntennaTuning() const;   ///< Assign antenna tuning signal to interrupt pin.
    void disableAntennaTuning() const;  ///< Revert assignment of antenna tuning signal to interrupt pin.
    //
    void clearStatistics() const;       ///< Clear distance estimation statistics.
    //
    bool irqHigh() const;               ///< Check for interrupt request signal.
    //
    InterruptType processIRQ(uint32_t& pEnergy, uint8_t& pDistance) const;  ///< Update values from chip according to interrupt type.

private:
    void writeReg(uint8_t pAddr, uint8_t pVal) const;   ///< Write value to a register.
    uint8_t readReg(uint8_t pAddr) const;               ///< Read value from a register.
    //
    void selectChip() const;                            ///< Enable device's SPI 'chip select' signal.
    void deselectChip() const;                          ///< Disable device's SPI 'chip select' signal.

private:
    const Pin spiChipSelectPin;     ///< Arduino pin used to connect SPI chip select pin to.
    const Pin irqPin;               ///< Arduino pin used to connect interrupt pin to.
    //
    const SPISettings spiSettings;  ///< Settings for SPI transactions.

private:
    static constexpr uint8_t registers[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                            0x30, 0x31, 0x32};                              ///< Addresses of all accessible registers.
    static constexpr uint8_t writableRegisters[] = {0x00, 0x01, 0x02, 0x03, 0x08};          ///< Addresses of all writable registers.

public:
    /*!
     * \brief Interrupt type to be read out after interrupt request signal.
     *
     * Interrupt requests are sent for different reasons (not only detected lightnings!).
     * This enum lists possible interrupt type register values.
     *
     * \note According to the datasheet you should wait 2ms after IRQ before reading the register.
     */
    enum class InterruptType : uint8_t
    {
        DistanceChanged = 0b0000,   ///< Distance estimation changed due to elapsed time (old events).
        Noise = 0b0001,             ///< Antenna signal is noisy.
        Disturber = 0b0100,         ///< Disturber signal detected.
        Lightning = 0b1000,         ///< Lightning detected.
        Invalid = 0b11111111        ///< Use this one for other (i.e. invalid) register values.
    };
    //
    static constexpr uint8_t stormDistanceOutOfRange = 63;  ///< Storm distance value used to report "out of range" condition.
};

#endif // AS3935_H

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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

#include <array>

/*!
 * \brief Register configuration object for the AS3935 registers.
 *
 * Stores the configurable settings of the AS3935 sensor in the form of a series of register values.
 *
 * The raw configuration (see load() and getRawConfig()) has a length of 24 bits and must
 * have the following format (always starting with MSB, bit lengths in parentheses):
 *
 * {RegIdent::AFE_GB (5), RegIdent::NF_LEV (3), RegIdent::WDTH (4), RegIdent::SREJ (4),
 *  RegIdent::TUN_CAP (4), RegIdent::LCO_FDIV (2), RegIdent::MIN_NUM_LIGH (2)}
 */
class Configuration
{
public:
    enum class RegIdent : uint8_t;

public:
    Configuration();                            ///< Constructor.
    //
    void load(std::array<bool, 24> pRawConf);   ///< Load configuration from raw DIP switch positions.
    //
    uint8_t getRegister(RegIdent pReg) const;   ///< Extract a register value from loaded raw configuration.
    std::array<bool, 24> getRawConfig() const;  ///< Get loaded raw configuration bits.

private:
    std::array<bool, 24> conf;  ///< Raw DIP configuration bits (mapped to registers via 'regOfsLenMap').

public:
    /*!
     * \brief %AS3935 registers configurable via Configuration.
     */
    enum class RegIdent : uint8_t
    {
        AFE_GB = 0,       ///< Analog front-end gain (indoor/outdoor) [5 bits].
        NF_LEV = 1,       ///< Noise floor level [3 bits].
        WDTH = 2,         ///< Watchdog threshold [4 bits].
        SREJ = 3,         ///< Spike rejection level [4 bits].
        TUN_CAP = 4,      ///< Antenna capacitor tuning value [4 bits].
        LCO_FDIV = 5,     ///< Divsion ratio for antenna resonance on interrupt pin (tuning mode) [2 bits].
        MIN_NUM_LIGH = 6  ///< Minimum number of lightnings to trigger an interrupt [2 bits].
    };

private:
    static constexpr uint8_t regOfsLenMap[][3] = {{static_cast<uint8_t>(RegIdent::AFE_GB), 0, 5},
                                                  {static_cast<uint8_t>(RegIdent::NF_LEV), 5, 3},
                                                  {static_cast<uint8_t>(RegIdent::WDTH), 8, 4},
                                                  {static_cast<uint8_t>(RegIdent::SREJ), 12, 4},
                                                  {static_cast<uint8_t>(RegIdent::TUN_CAP), 16, 4},
                                                  {static_cast<uint8_t>(RegIdent::LCO_FDIV), 20, 2},
                                                  {static_cast<uint8_t>(RegIdent::MIN_NUM_LIGH), 22, 2}};   ///< \brief Maps the configurable
                                                                                                            ///  %AS3935 registers to their
                                                                                                            ///  lengths and their offsets
                                                                                                            ///  within the 'conf' array.
};

#endif // CONFIGURATION_H

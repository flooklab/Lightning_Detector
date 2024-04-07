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

#include "configuration.h"

constexpr uint8_t Configuration::regOfsLenMap[][3];

//

/*!
 * \brief Constructor.
 *
 * Starts with all 24 configuration bits equal to 0.
 */
Configuration::Configuration() :
    conf()
{
    std::array<bool, 24> tConf;
    tConf.fill(0);
    load(std::move(tConf));
}

//Public

/*!
 * \brief Load configuration from raw DIP switch positions.
 *
 * See also getRawConfig().
 *
 * \param pRawConf The 24 configuration bits from the 24 switch positions.
 */
void Configuration::load(std::array<bool, 24> pRawConf)
{
    conf.swap(pRawConf);
}

//
/*!
 * \brief Extract a register value from loaded raw configuration.
 *
 * \param pReg Desired register.
 * \return Value of the register.
 */
uint8_t Configuration::getRegister(RegIdent pReg) const
{
    uint8_t retVal = 0;

    uint8_t ofs = regOfsLenMap[static_cast<uint8_t>(pReg)][1];
    uint8_t len = regOfsLenMap[static_cast<uint8_t>(pReg)][2];

    for (uint16_t i = ofs; i < ofs+len; ++i)
    {
        retVal = (retVal << 1);
        if (conf[i])
            retVal |= static_cast<uint8_t>(1);
    }

    return retVal;
}

/*!
 * \brief Get loaded raw configuration bits.
 *
 * See also load().
 *
 * \return The 24 configuration bits for the 24 switch positions.
 */
std::array<bool, 24> Configuration::getRawConfig() const
{
    return conf;
}

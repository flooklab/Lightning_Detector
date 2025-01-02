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

#ifndef AUXIL_H
#define AUXIL_H

#include <Arduino.h>

#include <algorithm>

/*!
 * \brief Auxiliary functions and other generic or convenient definitions.
 */
namespace Auxil
{

using ISRCallbackPtr = void (*const)(void);     ///< Callback function pointer type for interrupt service routines.

//

/*!
 * \brief Possible run modes of the Lightning Detector.
 *
 * Defines the different modes in which the firmware may be requested to run at startup.
 */
enum class RunMode : uint8_t
{
    Normal = 0,             ///< Normal lightning detection mode.
    UnmaskDisturbers = 1,   ///< Same as RunMode::Normal, except for unmasked disturbers and enabled serial.
    TuneAntenna = 2         ///< Antenna tuning mode.
};

//

float calcBatteryPercentage(float pCurrentVoltage, float pHighVoltage, float pLowVoltage,
                            float pLoadCurrentIdle, float pLoadCurrentMax, size_t pNumCoinCells);   ///< Estimate the battery percentage.

//

/*!
 * \brief Check if a C-style array contains a value.
 *
 * Checks if the C-style array \p pArray contains the value \p pValue.
 *
 * \tparam T Array type.
 * \tparam V Value type of \p T.
 * \param pArray A C-style array.
 * \param pValue Value to check for.
 * \return If \p pValue found in \p pArray.
 */
template<typename T, typename V = typename std::remove_extent_t<T>, size_t N = std::extent<T>::value>
bool arrayContains(const T& pArray, const V& pValue)
{
    return std::find(pArray, pArray+N, pValue) != pArray+N;
}

} // namespace Auxil

#endif // AUXIL_H

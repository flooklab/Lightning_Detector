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

#ifndef MUXEDDIPSWITCH_H
#define MUXEDDIPSWITCH_H

#include "pins.h"

#include <Arduino.h>

#include <array>

/*!
 * \brief Driver class for muxed \p M x \p N readout of \p M * \p N DIP switch bits.
 *
 * There are supposed to be \p M "select" pins and \p N "sense" pins. Each "select" pin must be simultaneously
 * connected to \p N different "sense" pins with the DIP switch in between "select" and "sense" and also
 * \p N Schottky diodes between "select" and the \p N switch pins with the cathode facing the switch side.
 *
 * The switch connected to "select" pin 0 and "sense" pin 0 is assumed to represent the MSB
 * and, likewise, the LSB is represented by "select" pin \p M-1 and "sense" pin \p N-1.
 *
 * \attention You must call setup() before using the class.
 */
template<size_t M, size_t N>
class MuxedDIPSwitch
{
public:
    /*!
     * \brief Constructor.
     *
     * \param pSelPins Arduino pins to use for "select" pins as {sel[0], ..., sel[\p M-1]}.
     * \param pSensPins Arduino pins to use for "sense" pins as {sens[0], ..., sens[\p N-1]}.
     */
    MuxedDIPSwitch(std::array<Pin, M> pSelPins, std::array<Pin, N> pSensPins) :
        selPins(pSelPins),
        sensPins(pSensPins)
    {
    }
    //
    /*!
     * \copybrief AS3935::setup()
     *
     * Sets all "select" and "sense" pins to input mode with pull-down enabled.
     */
    void setup() const
    {
        for (size_t i = 0; i < M; ++i)
            pinMode(selPins[i], INPUT_PULLDOWN);
        for (size_t i = 0; i < N; ++i)
            pinMode(sensPins[i], INPUT_PULLDOWN);
    }
    //
    /*!
     * \brief Read all N bits for a single select block.
     *
     * \param pNumBlock Number of "select" pin connected to desired bits to read.
     * \return Read bits for "select" block \p pNumBlock as {sens[0], ..., sens[N-1]}.
     */
    std::array<bool, N> readSelBlock(size_t pNumBlock) const
    {
        //Disable any previous selection
        for (size_t i = 0; i < M; ++i)
            pinMode(selPins[i], INPUT_PULLDOWN);

        //Select sel block
        pinMode(selPins[pNumBlock], OUTPUT);
        digitalWrite(selPins[pNumBlock], HIGH);

        //Paranoid
        delayMicroseconds(10);

        //Read sens values

        std::array<bool, N> retVal;

        for (size_t i = 0; i < N; ++i)
            retVal[i] = (digitalRead(sensPins[i]) == HIGH);

        //Disable selection
        digitalWrite(selPins[pNumBlock], LOW);
        pinMode(selPins[pNumBlock], INPUT_PULLDOWN);

        return retVal;
    }
    /*!
     * \brief Read all M*N bits.
     *
     * \return Read bits as {sel[0] X sens[0], sel[0] X sens[1], ..., sel[M-1] X sens[N-1]}.
     */
    std::array<bool, M*N> readAllBits() const
    {
        std::array<bool, M*N> retVal;

        for (size_t i = 0; i < M; ++i)
        {
            std::array<bool, N> blockVals = readSelBlock(i);
            for (size_t j = 0; j < N; ++j)
                retVal[i*N+j] = blockVals[j];
        }

        return retVal;
    }

private:
    const std::array<Pin, M> selPins;   ///< Select pins with "most significant" select block first (at index 0).
    const std::array<Pin, N> sensPins;  ///< Sense pins with "most significant" sense block first (at index 0).
};

#endif // MUXEDDIPSWITCH_H

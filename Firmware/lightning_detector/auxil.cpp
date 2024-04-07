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

#include "auxil.h"

#include <functional>

namespace Auxil
{

/*!
 * \brief Estimate the battery percentage.
 *
 * Estimates the battery charge from the measured voltage using an approximate look-up table
 * that was matched to \p CR2032 discharge curves available online. This takes into account
 * - the battery's changing internal resistance and number of parallel batteries (\p pNumCoinCells),
 * - the expected load current during the voltage measurement (\p pLoadCurrentIdle),
 * - the maximum expected current to determine a conservative 0 percent value (\p pLoadCurrentMax).
 *
 * The charge state when the voltage would drop below \p pLowVoltage given a load current
 * of \p pLoadCurrentMax results in a return value of 0, while a full battery under an
 * "idle" load current of \p pLoadCurrentIdle should yield a return value of 100.
 *
 * \param pCurrentVoltage Currently measured voltage in idle state.
 * \param pHighVoltage Maximum open-circuit battery voltage (i.e. ~ nominal voltage).
 * \param pLowVoltage Minimum required voltage.
 * \param pLoadCurrentIdle Approximate idle state current.
 * \param pLoadCurrentMax Approximate maximum current.
 * \param pNumCoinCells Number of parallel batteries used.
 * \return Estimated percentage value of remaining battery charge.
 */
float calcBatteryPercentage(float pCurrentVoltage, float pHighVoltage, float pLowVoltage,
                            float pLoadCurrentIdle, float pLoadCurrentMax, size_t pNumCoinCells)
{
    constexpr float chargeStateLUT[] = {3.000000, 2.999624, 2.999248, 2.998872, 2.998495, 2.998120, 2.997743, 2.997367, 2.996990,
        2.996614, 2.996237, 2.995861, 2.995484, 2.995107, 2.994730, 2.994353, 2.993976, 2.993599, 2.993221, 2.992844, 2.992466,
        2.992089, 2.991711, 2.991334, 2.990955, 2.990578, 2.990199, 2.989821, 2.989442, 2.989064, 2.988685, 2.988306, 2.987926,
        2.987548, 2.987168, 2.986789, 2.986408, 2.986029, 2.985648, 2.985269, 2.984888, 2.984507, 2.984126, 2.983745, 2.983363,
        2.982982, 2.982600, 2.982218, 2.981835, 2.981453, 2.981070, 2.980687, 2.980304, 2.979919, 2.979536, 2.979151, 2.978767,
        2.978381, 2.977996, 2.977610, 2.977224, 2.976837, 2.976450, 2.976062, 2.975675, 2.975286, 2.974898, 2.974508, 2.974118,
        2.973728, 2.973337, 2.972946, 2.972554, 2.972161, 2.971769, 2.971374, 2.970981, 2.970585, 2.970190, 2.969793, 2.969397,
        2.968998, 2.968600, 2.968200, 2.967801, 2.967399, 2.966998, 2.966594, 2.966191, 2.965786, 2.965381, 2.964974, 2.964566,
        2.964157, 2.963748, 2.963336, 2.962924, 2.962510, 2.962096, 2.961679, 2.961262, 2.960844, 2.960423, 2.960002, 2.959578,
        2.959153, 2.958726, 2.958298, 2.957868, 2.957436, 2.957002, 2.956567, 2.956129, 2.955689, 2.955247, 2.954804, 2.954357,
        2.953909, 2.953457, 2.953004, 2.952547, 2.952089, 2.951627, 2.951163, 2.950696, 2.950226, 2.949752, 2.949276, 2.948796,
        2.948313, 2.947826, 2.947336, 2.946841, 2.946343, 2.945841, 2.945335, 2.944824, 2.944309, 2.943789, 2.943266, 2.942736,
        2.942202, 2.941663, 2.941119, 2.940568, 2.940013, 2.939451, 2.938883, 2.938309, 2.937729, 2.937142, 2.936548, 2.935947,
        2.935338, 2.934723, 2.934098, 2.933467, 2.932826, 2.932178, 2.931520, 2.930853, 2.930176, 2.929491, 2.928794, 2.928088,
        2.927370, 2.926642, 2.925901, 2.925150, 2.924385, 2.923609, 2.922819, 2.922017, 2.921199, 2.920368, 2.919522, 2.918661,
        2.917783, 2.916890, 2.915980, 2.915053, 2.914107, 2.913144, 2.912160, 2.911158, 2.910135, 2.909091, 2.908025, 2.906938,
        2.905827, 2.904692, 2.903532, 2.902348, 2.901137, 2.899899, 2.898633, 2.897338, 2.896013, 2.894658, 2.893271, 2.891852,
        2.890398, 2.888909, 2.887385, 2.885823, 2.884223, 2.882583, 2.880903, 2.879180, 2.877413, 2.875601, 2.873744, 2.871837,
        2.869882, 2.867875, 2.865816, 2.863701, 2.861532, 2.859303, 2.857015, 2.854664, 2.852251, 2.849770, 2.847222, 2.844603,
        2.841913, 2.839146, 2.836304, 2.833380, 2.830375, 2.827284, 2.824106, 2.820836, 2.817474, 2.814014, 2.810456, 2.806793,
        2.803025, 2.799147, 2.795156, 2.791047, 2.786818, 2.782464, 2.777982, 2.773366, 2.768614, 2.763719, 2.758680, 2.753488,
        2.748142, 2.742634, 2.736961, 2.731116, 2.725094, 2.718890, 2.712496, 2.705909, 2.699120, 2.692125, 2.684915, 2.677484,
        2.669824, 2.661930, 2.653792, 2.645403, 2.636755, 2.627840, 2.618647, 2.609171, 2.599399, 2.589325, 2.578935, 2.568223,
        2.557176, 2.545785, 2.534036, 2.521920, 2.509424, 2.496537, 2.483245, 2.469535, 2.455394, 2.440808, 2.425762, 2.410242,
        2.394232, 2.377717, 2.360679, 2.343104, 2.324971, 2.306265, 2.286965, 2.267055, 2.246511, 2.225317, 2.203448, 2.180886,
        2.157605, 2.133584, 2.108797, 2.083223};

    constexpr size_t lutLength = std::extent<decltype(chargeStateLUT)>::value;

    constexpr float innerResistanceLUT[lutLength] = {10.00000, 10.02840, 10.05671, 10.08511, 10.11352, 10.14182, 10.17023,
        10.19863, 10.22703, 10.25544, 10.28384, 10.31225, 10.34065, 10.36915, 10.39756, 10.42606, 10.45446, 10.48297, 10.51147,
        10.53997, 10.56848, 10.59698, 10.62548, 10.65399, 10.68259, 10.71109, 10.73970, 10.76830, 10.79690, 10.82550, 10.85411,
        10.88281, 10.91151, 10.94011, 10.96882, 10.99752, 11.02632, 11.05502, 11.08382, 11.11252, 11.14132, 11.17023, 11.19903,
        11.22793, 11.25683, 11.28573, 11.31463, 11.34363, 11.37263, 11.40163, 11.43063, 11.45973, 11.48883, 11.51803, 11.54712,
        11.57632, 11.60552, 11.63482, 11.66412, 11.69342, 11.72281, 11.75221, 11.78171, 11.81120, 11.84070, 11.87029, 11.89989,
        11.92959, 11.95938, 11.98908, 12.01897, 12.04876, 12.07876, 12.10875, 12.13874, 12.16893, 12.19903, 12.22932, 12.25961,
        12.29000, 12.32039, 12.35098, 12.38157, 12.41226, 12.44294, 12.47383, 12.50472, 12.53580, 12.56689, 12.59807, 12.62936,
        12.66074, 12.69232, 12.72391, 12.75559, 12.78747, 12.81945, 12.85152, 12.88370, 12.91608, 12.94855, 12.98113, 13.01390,
        13.04678, 13.07985, 13.11312, 13.14649, 13.18006, 13.21373, 13.24769, 13.28176, 13.31602, 13.35048, 13.38524, 13.42010,
        13.45516, 13.49052, 13.52607, 13.56192, 13.59797, 13.63432, 13.67087, 13.70772, 13.74486, 13.78220, 13.81994, 13.85798,
        13.89632, 13.93495, 13.97398, 14.01331, 14.05303, 14.09316, 14.13368, 14.17450, 14.21581, 14.25752, 14.29973, 14.34234,
        14.38534, 14.42894, 14.47304, 14.51753, 14.56262, 14.60830, 14.65448, 14.70126, 14.74873, 14.79670, 14.84537, 14.89473,
        14.94468, 14.99543, 15.04688, 15.09902, 15.15205, 15.20578, 15.26040, 15.31582, 15.37213, 15.42944, 15.48764, 15.54673,
        15.60691, 15.66809, 15.73036, 15.79372, 15.85828, 15.92392, 15.99086, 16.05899, 16.12841, 16.19913, 16.27133, 16.34492,
        16.41990, 16.49647, 16.57464, 16.65438, 16.73572, 16.81885, 16.90376, 16.99047, 17.07915, 17.16973, 17.26229, 17.35704,
        17.45387, 17.55289, 17.65419, 17.75797, 17.86414, 17.97279, 18.08402, 18.19803, 18.31473, 18.43440, 18.55696, 18.68259,
        18.81130, 18.94329, 19.07876, 19.21760, 19.36002, 19.50611, 19.65607, 19.80991, 19.96782, 20.12990, 20.29645, 20.46738,
        20.64286, 20.82322, 21.00844, 21.19873, 21.39428, 21.59529, 21.80177, 22.01410, 22.23240, 22.45685, 22.68756, 22.92492,
        23.16903, 23.42010, 23.67832, 23.94409, 24.21740, 24.49876, 24.78826, 25.08621, 25.39289, 25.70861, 26.03357, 26.36816,
        26.71258, 27.06734, 27.43261, 27.80872, 28.19615, 28.59519, 29.00626, 29.42964, 29.86583, 30.31522, 30.77823, 31.25534,
        31.74685, 32.25345, 32.77545, 33.31344, 33.86791, 34.43947, 35.02850, 35.63571, 36.26169, 36.90694, 37.57215, 38.25792,
        38.96494, 39.69401, 40.44572, 41.22078, 42.01996, 42.84418, 43.69401, 44.57046, 45.47443, 46.40659, 47.36806, 48.35962,
        49.38246, 50.43738, 51.52547, 52.64783, 53.80564, 55.00000, 56.23200, 57.50293, 58.81398, 60.16665, 61.56202, 63.00169,
        64.48694, 66.01937, 67.60036, 69.23160, 70.91469, 72.65121, 74.44304, 76.29189, 78.19962, 80.16804, 82.19932, 84.29526,
        86.45814, 88.68994, 90.99305, 93.36975, 95.82243, 98.35336};

    auto findChargeState = [chargeStateLUT = std::cref(chargeStateLUT), innerResistanceLUT = std::cref(innerResistanceLUT),
                            lutLength, pNumCoinCells](float pVoltage, float pLoadCurrent) -> size_t
    {
        for (size_t i = 0; i < lutLength; ++i)
            if ((chargeStateLUT[i] - pLoadCurrent*innerResistanceLUT[i]/pNumCoinCells) < pVoltage)
                return i;

        return lutLength;
    };

    size_t highChargeIdx = findChargeState(pHighVoltage, 0);
    size_t currentChargeIdx = findChargeState(pCurrentVoltage, pLoadCurrentIdle);
    size_t lowChargeIdx = findChargeState(pLowVoltage, pLoadCurrentMax);

    if (currentChargeIdx < highChargeIdx)
        currentChargeIdx = highChargeIdx;
    else if (currentChargeIdx > lowChargeIdx)
        currentChargeIdx = lowChargeIdx;

    return 100. * (static_cast<float>(lowChargeIdx - currentChargeIdx) / static_cast<float>(lowChargeIdx - highChargeIdx));
}

} // namespace Auxil

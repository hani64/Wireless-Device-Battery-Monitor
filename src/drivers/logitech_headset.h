#pragma once
// Voltage -> % for Logitech G headsets that only report a voltage (HID++ 0x1F20 ADC measurement).
// Kept independent of Win32 so the curves can be tested without a headset.
// Calibration: github.com/Sapd/HeadsetControl. PRO X Wireless: G HUB's power model for 046D:0ABA
// (pro_headset/pro_x_wireless_headset_battery.xml), checked against G HUB on a real headset (#3434).
#include <cstdint>

namespace LogitechHeadset {

// Piecewise-linear lookup, highest voltage first; same rounding as Interpolate() in hid.cpp.
inline int Interpolate(int mv, const int* mvTable, const int* pctTable, int n) {
    if (mv >= mvTable[0]) return pctTable[0];
    for (int i = 1; i < n; i++) {
        if (mv >= mvTable[i]) {
            int span = mvTable[i - 1] - mvTable[i];
            return pctTable[i] + (pctTable[i - 1] - pctTable[i]) * (mv - mvTable[i]) / (span ? span : 1);
        }
    }
    return pctTable[n - 1];
}

inline int Percent(std::uint16_t pid, int mv) {
    static const int g533v[] = { 4200, 3850, 3790, 3750, 3680, 3330 }, g533p[] = { 100, 50, 30, 20, 5, 0 };
    static const int g535v[] = { 4175, 3817, 3766, 3730, 3664, 3310 }, g535p[] = { 100, 50, 30, 20, 5, 0 };
    static const int g633v[] = { 4100, 3950, 3850, 3750, 3650, 3500, 3300, 3150 }, g633p[] = { 100, 80, 60, 40, 20, 10, 5, 0 };
    static const int gprov[] = { 4150, 3830, 3780, 3740, 3670, 3320 }, gprop[] = { 100, 50, 30, 20, 5, 0 };
    // PRO X Wireless: voltage at every 5% of remaining capacity on G HUB's discharge curve
    // (remaining = 1 - mAh drawn / 1271.36 mAh), which is how G HUB computes the level.
    static const int proxv[] = { 4102, 4046, 3993, 3950, 3915, 3877, 3838, 3809, 3789, 3772, 3758,
                                 3747, 3736, 3722, 3707, 3687, 3661, 3644, 3548, 3426, 3293 };
    static const int proxp[] = { 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50,
                                 45, 40, 35, 30, 25, 20, 15, 10, 5, 0 };
    static const int defv[] = { 4100, 3850, 3780, 3730, 3650, 3300 }, defp[] = { 100, 50, 30, 20, 5, 0 };
    switch (pid) {
        case 0x0A66: return Interpolate(mv, g533v, g533p, 6);
        case 0x0AC4: return Interpolate(mv, g535v, g535p, 6);
        case 0x0A5C: case 0x0A89: case 0x0A5B: case 0x0A87: case 0x0AB5: case 0x0AFE: return Interpolate(mv, g633v, g633p, 8);
        case 0x0AAA: case 0x0ABA: return Interpolate(mv, proxv, proxp, 21);
        case 0x0AA7: case 0x0AFB: return Interpolate(mv, gprov, gprop, 6);
        default: return Interpolate(mv, defv, defp, 6);
    }
}

}  // namespace LogitechHeadset

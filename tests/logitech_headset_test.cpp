#include "../src/drivers/logitech_headset.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

int main() {
    using LogitechHeadset::Percent;

    // PRO X Wireless (0AAA / 0ABA) follows G HUB's discharge curve (issue #3434).
    for (unsigned pid : { 0x0AAAu, 0x0ABAu }) {
        assert(Percent(pid, 4200) == 100 && Percent(pid, 4102) == 100);
        assert(Percent(pid, 3993) == 90);
        assert(Percent(pid, 3838) == 70);
        assert(Percent(pid, 3758) == 50);   // issue #549: G HUB 50%, old table 24%
        assert(Percent(pid, 3661) == 20);
        assert(Percent(pid, 3293) == 0 && Percent(pid, 3000) == 0);
    }
    // Paired readings on a real 0ABA (mV, G HUB %): within one point of G HUB.
    const int seen[][2] = { { 3971, 88 }, { 3946, 85 }, { 3909, 79 }, { 3879, 75 }, { 3842, 70 } };
    for (const auto& s : seen) assert(std::abs(Percent(0x0ABA, s[0]) - s[1]) <= 1);

    // Other G PRO headsets keep HeadsetControl's calibration until they are measured.
    assert(Percent(0x0AA7, 3830) == 50 && Percent(0x0AFB, 3830) == 50);
    assert(Percent(0x0AA7, 3758) == 24);

    std::puts("logitech_headset_test: ok");
    return 0;
}

#include "solarium/time/time_conversion.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    std::cout << "Running Time Conversion tests...\n";

    using namespace solarium::time;

    const JulianDate jd =
        TimeConversion::calendarToJulianDate(
            2000,
            1,
            1,
            12,
            0,
            0.0
        );

    assert(
        std::abs(jd.value() - 2451545.0)
        < 1e-9
    );

    std::cout << "Time Conversion tests passed.\n";

    return 0;
}
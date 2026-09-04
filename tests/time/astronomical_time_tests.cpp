#include "solarium/time/astronomical_time.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    std::cout << "Running Astronomical Time tests...\n";

    using namespace solarium::time;

    AstronomicalTime time(
        JulianDate(2451545.0),
        TimeScale::TDB
    );

    assert(std::abs(time.julianDay() - 2451545.0) < 1e-12);
    assert(time.scale() == TimeScale::TDB);

    time.advanceDays(2.5);

    assert(std::abs(time.julianDay() - 2451547.5) < 1e-12);

    std::cout << "Astronomical Time tests passed.\n";

    return 0;
}
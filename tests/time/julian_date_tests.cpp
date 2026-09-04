#include "solarium/time/julian_date.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    std::cout << "Running Julian Date tests...\n";

    solarium::time::JulianDate jd(2451545.0);

    assert(std::abs(jd.value() - 2451545.0) < 1e-12);

    jd += 1.0;

    assert(std::abs(jd.value() - 2451546.0) < 1e-12);

    auto next = jd + 10.0;

    assert(std::abs(next.value() - 2451556.0) < 1e-12);

    std::cout << "Julian Date tests passed.\n";

    return 0;
}
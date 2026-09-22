#include "solarium/time/time_conversion.hpp"
#include "solarium/time/time_scale_converter.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

int main() {
    using namespace solarium::time;

    const JulianDate leapBoundary = TimeConversion::calendarToJulianDate(
        2017, 1, 1, 0, 0, 0.0
    );
    const JulianDate beforeBoundary = TimeConversion::calendarToJulianDate(
        2016, 12, 31, 23, 59, 59.0
    );

    auto leapSeconds = std::make_shared<LeapSecondTable>(std::vector<LeapSecondEntry>{
        {TimeConversion::calendarToJulianDate(1972, 1, 1), 10},
        {TimeConversion::calendarToJulianDate(2015, 7, 1), 36},
        {leapBoundary, 37}
    });
    TimeScaleConverter converter(leapSeconds);

    assert(leapSeconds->taiMinusUtc(beforeBoundary) == 36);
    assert(leapSeconds->taiMinusUtc(leapBoundary) == 37);

    const JulianDate tt = converter.convert(
        leapBoundary,
        TimeScale::UTC,
        TimeScale::TT
    );
    assert(std::abs((tt - leapBoundary) * 86'400.0 - 69.184) < 1.0e-7);

    const JulianDate tai = converter.convert(
        beforeBoundary,
        TimeScale::UTC,
        TimeScale::TAI
    );
    const JulianDate utcRoundTrip = converter.convert(
        tai,
        TimeScale::TAI,
        TimeScale::UTC
    );
    assert(std::abs((utcRoundTrip - beforeBoundary) * 86'400.0) < 1.0e-6);

    const JulianDate tdb = converter.convert(
        tt,
        TimeScale::TT,
        TimeScale::TDB
    );
    const double tdbOffset = (tdb - tt) * 86'400.0;
    assert(std::abs(tdbOffset) > 1.0e-8);
    assert(std::abs(tdbOffset) < 0.002);

    const JulianDate ttRoundTrip = converter.convert(
        tdb,
        TimeScale::TDB,
        TimeScale::TT
    );
    assert(std::abs((ttRoundTrip - tt) * 86'400.0) < 1.0e-7);

    std::cout << "TimeScaleConverterTests passed.\n";
    return 0;
}

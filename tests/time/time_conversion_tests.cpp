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

    const CalendarDate calendar =
        TimeConversion::julianDateToCalendar(jd);

    assert(calendar.year == 2000);
    assert(calendar.month == 1);
    assert(calendar.day == 1);
    assert(calendar.hour == 12);
    assert(calendar.minute == 0);
    assert(std::abs(calendar.second) < 1e-12);

    const JulianDate precise =
        TimeConversion::calendarToJulianDate(
            2026,
            9,
            22,
            12,
            34,
            56.789123
        );
    const CalendarDate preciseCalendar =
        TimeConversion::julianDateToCalendar(precise);
    assert(preciseCalendar.year == 2026);
    assert(preciseCalendar.month == 9);
    assert(preciseCalendar.day == 22);
    assert(preciseCalendar.hour == 12);
    assert(preciseCalendar.minute == 34);
    assert(std::abs(preciseCalendar.second - 56.789123) < 1.0e-5);

    std::cout << "Time Conversion tests passed.\n";

    return 0;
}
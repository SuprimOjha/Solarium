#include "solarium/time/time_conversion.hpp"

#include <cmath>

namespace solarium::time {

JulianDate TimeConversion::calendarToJulianDate(
    int year,
    int month,
    int day,
    int hour,
    int minute,
    double second
) {
    int adjustedYear = year;
    int adjustedMonth = month;

    if (adjustedMonth <= 2) {
        --adjustedYear;
        adjustedMonth += 12;
    }

    const int century = adjustedYear / 100;

    const int gregorianCorrection =
        2 - century + century / 4;

    const double dayFraction =
        (static_cast<double>(hour) / 24.0) +
        (static_cast<double>(minute) / 1440.0) +
        (second / 86400.0);

    const double julianDay =
        std::floor(365.25 * (adjustedYear + 4716)) +
        std::floor(30.6001 * (adjustedMonth + 1)) +
        static_cast<double>(day) +
        static_cast<double>(gregorianCorrection) -
        1524.5 +
        dayFraction;

    return JulianDate(julianDay);
}

}
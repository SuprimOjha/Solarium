#include "solarium/time/time_conversion.hpp"

#include <cmath>
#include <cstdint>

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

    const long double dayFraction =
        (static_cast<long double>(hour) / 24.0L) +
        (static_cast<long double>(minute) / 1440.0L) +
        (static_cast<long double>(second) / 86400.0L);

    const long double julianDay =
        std::floor(365.25L * (adjustedYear + 4716)) +
        std::floor(30.6001L * (adjustedMonth + 1)) +
        static_cast<long double>(day) +
        static_cast<long double>(gregorianCorrection) -
        1524.5L +
        dayFraction;

    const auto wholeDay = static_cast<std::int64_t>(std::floor(julianDay));
    return JulianDate::fromParts(
        wholeDay,
        static_cast<double>(julianDay - static_cast<long double>(wholeDay))
    );
}

CalendarDate TimeConversion::julianDateToCalendar(JulianDate julianDate) {
    const long double value =
        static_cast<long double>(julianDate.wholeDay()) +
        static_cast<long double>(julianDate.fractionalDay());
    const long double shifted = value + 0.5L;
    const auto integerPart = static_cast<std::int64_t>(std::floor(shifted));
    const long double fraction = shifted - static_cast<long double>(integerPart);

    std::int64_t yearPart = integerPart;
    if (integerPart >= 2'299'161) {
        const auto alpha = static_cast<std::int64_t>(
            std::floor((static_cast<long double>(integerPart) - 1'867'216.25L) / 36'524.25L)
        );
        yearPart += 1 + alpha - alpha / 4;
    }

    const auto b = yearPart + 1'524;
    const auto c = static_cast<std::int64_t>(
        std::floor((static_cast<long double>(b) - 122.1L) / 365.25L)
    );
    const auto d = static_cast<std::int64_t>(std::floor(365.25L * c));
    const auto e = static_cast<std::int64_t>(
        std::floor(static_cast<long double>(b - d) / 30.6001L)
    );

    CalendarDate result;
    result.day = static_cast<int>(b - d - static_cast<std::int64_t>(std::floor(30.6001L * e)));
    result.month = static_cast<int>(e < 14 ? e - 1 : e - 13);
    result.year = static_cast<int>(result.month > 2 ? c - 4716 : c - 4715);

    const long double totalSeconds = fraction * 86'400.0L;
    result.hour = static_cast<int>(totalSeconds / 3'600.0L);
    const long double afterHour = totalSeconds - result.hour * 3'600.0L;
    result.minute = static_cast<int>(afterHour / 60.0L);
    result.second = static_cast<double>(afterHour - result.minute * 60.0L);
    return result;
}

}
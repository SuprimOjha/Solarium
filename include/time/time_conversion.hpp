#pragma once

#include "solarium/time/julian_date.hpp"

namespace solarium::time {

class TimeConversion {
public:
    [[nodiscard]]
    static JulianDate calendarToJulianDate(
        int year,
        int month,
        int day,
        int hour = 0,
        int minute = 0,
        double second = 0.0
    );

private:
    TimeConversion() = delete;
};

}
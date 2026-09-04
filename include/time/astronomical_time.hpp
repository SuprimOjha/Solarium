#pragma once

#include "solarium/time/julian_date.hpp"
#include "solarium/time/time_scale.hpp"

namespace solarium::time {

class AstronomicalTime {
public:
    AstronomicalTime() noexcept;

    explicit AstronomicalTime(
        JulianDate julianDate,
        TimeScale scale = TimeScale::TDB
    ) noexcept;

    [[nodiscard]]
    JulianDate julianDate() const noexcept;

    [[nodiscard]]
    TimeScale scale() const noexcept;

    [[nodiscard]]
    double julianDay() const noexcept;

    void advanceDays(double days) noexcept;

private:
    JulianDate julianDate_;
    TimeScale scale_;
};

}
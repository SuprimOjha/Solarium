#include "solarium/time/astronomical_time.hpp"

namespace solarium::time {

AstronomicalTime::AstronomicalTime() noexcept
    : julianDate_(JulianDate(0.0)),
      scale_(TimeScale::TDB) {}

AstronomicalTime::AstronomicalTime(
    JulianDate julianDate,
    TimeScale scale
) noexcept
    : julianDate_(julianDate),
      scale_(scale) {}

JulianDate AstronomicalTime::julianDate() const noexcept {
    return julianDate_;
}

TimeScale AstronomicalTime::scale() const noexcept {
    return scale_;
}

double AstronomicalTime::julianDay() const noexcept {
    return julianDate_.value();
}

void AstronomicalTime::advanceDays(double days) noexcept {
    julianDate_ += days;
}

}
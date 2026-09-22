#pragma once

#include "solarium/time/leap_second_provider.hpp"
#include "solarium/time/time_scale.hpp"

#include <memory>

namespace solarium::time {

class TimeScaleConverter {
public:
    explicit TimeScaleConverter(std::shared_ptr<const LeapSecondProvider> leapSeconds);

    [[nodiscard]] JulianDate convert(
        JulianDate epoch,
        TimeScale from,
        TimeScale to
    ) const;

    [[nodiscard]] double tdbMinusTtSeconds(JulianDate tt) const noexcept;
    [[nodiscard]] const LeapSecondProvider& leapSeconds() const noexcept;

private:
    std::shared_ptr<const LeapSecondProvider> leapSeconds_;

    [[nodiscard]] JulianDate toTai(JulianDate epoch, TimeScale scale) const;
    [[nodiscard]] JulianDate fromTai(JulianDate tai, TimeScale scale) const;
};

} // namespace solarium::time

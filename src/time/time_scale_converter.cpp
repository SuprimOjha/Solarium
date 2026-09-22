#include "solarium/time/time_scale_converter.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace solarium::time {
namespace {

constexpr double SecondsPerDay = 86'400.0;
constexpr double TtMinusTaiSeconds = 32.184;
constexpr double Pi = 3.141592653589793238462643383279502884;

[[nodiscard]] JulianDate addSeconds(JulianDate epoch, double seconds) noexcept {
    return epoch + seconds / SecondsPerDay;
}

} // namespace

TimeScaleConverter::TimeScaleConverter(
    std::shared_ptr<const LeapSecondProvider> leapSeconds
)
    : leapSeconds_(std::move(leapSeconds)) {
    if (!leapSeconds_) {
        throw std::invalid_argument("a leap-second provider is required");
    }
}

JulianDate TimeScaleConverter::convert(
    JulianDate epoch,
    TimeScale from,
    TimeScale to
) const {
    if (from == to) {
        return epoch;
    }
    return fromTai(toTai(epoch, from), to);
}

double TimeScaleConverter::tdbMinusTtSeconds(JulianDate tt) const noexcept {
    // IAU SOFA-style low-order periodic approximation; replaceable by a higher
    // fidelity ephemeris implementation without changing the conversion API.
    const double centuries = (tt.value() - 2'451'545.0) / 36'525.0;
    const double meanAnomaly =
        (357.53 + 35'999.05034 * centuries) * Pi / 180.0;
    return 0.001657 * std::sin(meanAnomaly) +
        0.00001385 * std::sin(2.0 * meanAnomaly);
}

const LeapSecondProvider& TimeScaleConverter::leapSeconds() const noexcept {
    return *leapSeconds_;
}

JulianDate TimeScaleConverter::toTai(JulianDate epoch, TimeScale scale) const {
    switch (scale) {
    case TimeScale::UTC:
        return addSeconds(epoch, static_cast<double>(leapSeconds_->taiMinusUtc(epoch)));
    case TimeScale::TAI:
        return epoch;
    case TimeScale::TT:
        return addSeconds(epoch, -TtMinusTaiSeconds);
    case TimeScale::TDB: {
        JulianDate tt = epoch;
        for (int iteration = 0; iteration < 3; ++iteration) {
            tt = addSeconds(epoch, -tdbMinusTtSeconds(tt));
        }
        return addSeconds(tt, -TtMinusTaiSeconds);
    }
    }
    throw std::invalid_argument("unknown source time scale");
}

JulianDate TimeScaleConverter::fromTai(JulianDate tai, TimeScale scale) const {
    switch (scale) {
    case TimeScale::UTC: {
        JulianDate utc = tai;
        for (int iteration = 0; iteration < 3; ++iteration) {
            utc = addSeconds(
                tai,
                -static_cast<double>(leapSeconds_->taiMinusUtc(utc))
            );
        }
        return utc;
    }
    case TimeScale::TAI:
        return tai;
    case TimeScale::TT:
        return addSeconds(tai, TtMinusTaiSeconds);
    case TimeScale::TDB: {
        const JulianDate tt = addSeconds(tai, TtMinusTaiSeconds);
        return addSeconds(tt, tdbMinusTtSeconds(tt));
    }
    }
    throw std::invalid_argument("unknown target time scale");
}

} // namespace solarium::time

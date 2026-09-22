#pragma once

#include "solarium/ephemeris/spice_kernel_manager.hpp"
#include "solarium/reference/reference_frame.hpp"
#include "solarium/time/julian_date.hpp"
#include "solarium/time/leap_second_provider.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace solarium::ephemeris {

struct SpiceState {
    double positionKilometers[3]{};
    double velocityKilometersPerSecond[3]{};
    std::string frame;
    int centerNaifId = 0;
    double ephemerisTime = 0.0;
};

class SpiceStateBackend {
public:
    virtual ~SpiceStateBackend() = default;

    [[nodiscard]] virtual double toEphemerisTime(
        time::JulianDate epoch,
        TimeScale timeScale
    ) const = 0;

    [[nodiscard]] virtual SpiceState queryState(
        int targetNaifId,
        int centerNaifId,
        double ephemerisTime,
        std::string_view frameName
    ) const = 0;

    [[nodiscard]] virtual std::int32_t taiMinusUtcFromLsk(
        time::JulianDate utc
    ) const;
};

class CspiceStateBackend final : public SpiceStateBackend {
public:
    [[nodiscard]] double toEphemerisTime(
        time::JulianDate epoch,
        TimeScale timeScale
    ) const override;

    [[nodiscard]] SpiceState queryState(
        int targetNaifId,
        int centerNaifId,
        double ephemerisTime,
        std::string_view frameName
    ) const override;

    [[nodiscard]] std::int32_t taiMinusUtcFromLsk(
        time::JulianDate utc
    ) const override;
};

class SpiceLeapSecondProvider final : public time::LeapSecondProvider {
public:
    SpiceLeapSecondProvider(
        std::shared_ptr<const SpiceStateBackend> backend,
        time::JulianDate coverageStart,
        time::JulianDate coverageEnd
    );

    [[nodiscard]] std::int32_t taiMinusUtc(time::JulianDate utc) const override;
    [[nodiscard]] time::JulianDate coverageStart() const noexcept override;
    [[nodiscard]] time::JulianDate coverageEnd() const noexcept override;
    [[nodiscard]] std::string_view source() const noexcept override;

private:
    std::shared_ptr<const SpiceStateBackend> backend_;
    time::JulianDate coverageStart_;
    time::JulianDate coverageEnd_;
};

} // namespace solarium::ephemeris

#pragma once

#include "solarium/ephemeris/spice_kernel_manager.hpp"
#include "solarium/reference/reference_frame.hpp"
#include "solarium/time/julian_date.hpp"

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
};

} // namespace solarium::ephemeris

#pragma once

#include "solarium/ephemeris/canonical_state.hpp"

#include <optional>
#include <string_view>

namespace solarium::ephemeris {

struct EphemerisRequest {
    BodyId body;
    time::JulianDate epoch;
    std::optional<BodyId> center;
    reference::ReferenceFrame frame;
    TimeScale timeScale;
};

class EphemerisProvider {
public:
    virtual ~EphemerisProvider() = default;

    [[nodiscard]] virtual CanonicalState getState(
        const EphemerisRequest& request
    ) const {
        return getState(request.body, request.epoch, request.frame);
    }

    [[nodiscard]] virtual CanonicalState getState(
        BodyId body,
        time::JulianDate epoch,
        reference::ReferenceFrame frame
    ) const = 0;

    [[nodiscard]] virtual std::string_view providerId() const noexcept = 0;
};

} // namespace solarium::ephemeris

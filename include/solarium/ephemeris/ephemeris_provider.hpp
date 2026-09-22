#pragma once

#include "solarium/ephemeris/canonical_state.hpp"

#include <string_view>

namespace solarium::ephemeris {

class EphemerisProvider {
public:
    virtual ~EphemerisProvider() = default;

    [[nodiscard]] virtual CanonicalState getState(
        BodyId body,
        time::JulianDate epoch,
        reference::ReferenceFrame frame
    ) const = 0;

    [[nodiscard]] virtual std::string_view providerId() const noexcept = 0;
};

} // namespace solarium::ephemeris

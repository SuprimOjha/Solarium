#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/orbital/orbital_elements.hpp"

namespace solarium::orbital {

class OrbitalConverter {
public:

    [[nodiscard]]
    static OrbitalElements fromState(
        const math::Vec3& position,
        const math::Vec3& velocity,
        double gravitationalParameter
    );

};

} // namespace solarium::orbital
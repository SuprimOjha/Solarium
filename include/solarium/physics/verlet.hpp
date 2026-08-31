#pragma once

#include "solarium/celestial/celestial_body.hpp"

namespace solarium::physics {

class VelocityVerlet {
public:
    static void integrate(
        celestial::CelestialBody& body,
        const math::Vec3& newAcceleration,
        double dt
    );
};

} // namespace solarium::physics
#pragma once

#include "solarium/celestial/celestial_body.hpp"

#include <vector>

namespace solarium::physics {

class NBodySolver {
public:

    void computeAccelerations(
        std::vector<
            celestial::CelestialBody
        >& bodies
    ) const;

private:

    [[nodiscard]]
    static math::Vec3 accelerationFromBody(
        const celestial::CelestialBody& source,
        const celestial::CelestialBody& target
    );
};

} // namespace solarium::physics
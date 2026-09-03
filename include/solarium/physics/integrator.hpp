#pragma once

#include "solarium/celestial/celestial_body.hpp"

#include <vector>

namespace solarium::physics {

class Integrator {
public:
    virtual ~Integrator() = default;

    virtual void step(
        std::vector<celestial::CelestialBody>& bodies,
        double deltaTime
    ) = 0;
};

} // namespace solarium::physics
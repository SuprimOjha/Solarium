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
};

}
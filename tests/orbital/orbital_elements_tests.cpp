#include "solarium/orbital/orbital_converter.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    using namespace solarium;

    /*
     * Normalized two-body system
     *
     * mu = 1
     * r  = 1
     * circular velocity = 1
     */

    constexpr double mu = 1.0;

    const math::Vec3 position(
        1.0,
        0.0,
        0.0
    );

    const math::Vec3 velocity(
        0.0,
        1.0,
        0.0
    );

    const orbital::OrbitalElements elements =
        orbital::OrbitalConverter::fromState(
            position,
            velocity,
            mu
        );

    assert(
        std::abs(
            elements.semiMajorAxis - 1.0
        ) < 1e-12
    );

    assert(
        std::abs(
            elements.eccentricity
        ) < 1e-12
    );

    assert(
        std::abs(
            elements.inclination
        ) < 1e-12
    );

    assert(
        std::abs(
            elements.orbitalEnergy + 0.5
        ) < 1e-12
    );

    assert(
        std::abs(
            elements.specificAngularMomentum - 1.0
        ) < 1e-12
    );

    std::cout
        << "OrbitalElementsTests passed.\n";

    return 0;
}
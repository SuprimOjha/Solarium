#pragma once

#include "solarium/celestial/celestial_body.hpp"

#include <vector>

namespace solarium::physics {

struct PhysicsState {

    double kineticEnergy;
    double potentialEnergy;
    double totalEnergy;

    math::Vec3 linearMomentum;
    math::Vec3 angularMomentum;
};

class PhysicsDiagnostics {
public:

    [[nodiscard]]
    static PhysicsState calculate(
        const std::vector<
            celestial::CelestialBody
        >& bodies
    );

    [[nodiscard]]
    static double relativeEnergyError(
        double initialEnergy,
        double currentEnergy
    ) noexcept;
};

} // namespace solarium::physics
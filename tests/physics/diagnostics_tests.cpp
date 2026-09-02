#include "solarium/celestial/body_registry.hpp"
#include "solarium/physics/physics_diagnostics.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    using namespace solarium;

    celestial::BodyRegistry registry;

    registry.initializeSolarSystem();

    const auto& bodies =
        registry.bodies();

    const physics::PhysicsState state =
        physics::PhysicsDiagnostics::calculate(
            bodies
        );

    assert(
        std::isfinite(
            state.kineticEnergy
        )
    );

    assert(
        std::isfinite(
            state.potentialEnergy
        )
    );

    assert(
        std::isfinite(
            state.totalEnergy
        )
    );

    assert(
        state.kineticEnergy > 0.0
    );

    assert(
        state.potentialEnergy < 0.0
    );

    assert(
        state.totalEnergy < 0.0
    );

    std::cout
        << "DiagnosticsTests passed.\n";

    return 0;
}
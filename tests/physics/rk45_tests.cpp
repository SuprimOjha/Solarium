#include "solarium/celestial/body_registry.hpp"
#include "solarium/physics/n_body_solver.hpp"
#include "solarium/physics/rk45.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium;

    celestial::BodyRegistry registry;
    registry.initializeSolarSystem();

    auto& bodies = registry.bodies();

    physics::NBodySolver solver;
    physics::RK45Integrator integrator(solver);

    const math::Vec3 initialPosition =
        bodies[0].position();

    const math::Vec3 initialVelocity =
        bodies[0].velocity();

    integrator.step(
        bodies,
        1.0
    );

    const auto& result =
        integrator.lastResult();

    assert(std::isfinite(result.positionError));
    assert(std::isfinite(result.velocityError));
    assert(std::isfinite(result.maximumError));

    assert(result.positionError >= 0.0);
    assert(result.velocityError >= 0.0);
    assert(result.maximumError >= 0.0);

    assert(
        bodies[0].position().magnitude() > 0.0
    );

    assert(
        bodies[0].velocity().magnitude() >= 0.0
    );

    // The state should actually change.
    const bool positionChanged =
        (
            bodies[0].position() -
            initialPosition
        ).magnitude() > 0.0;

    const bool velocityChanged =
        (
            bodies[0].velocity() -
            initialVelocity
        ).magnitude() > 0.0;

    assert(positionChanged || velocityChanged);

    std::cout
        << "RK45Tests passed.\n";

    std::cout
        << "Position error: "
        << result.positionError
        << '\n';

    std::cout
        << "Velocity error: "
        << result.velocityError
        << '\n';

    return 0;
}
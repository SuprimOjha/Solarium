#include "solarium/celestial/body_registry.hpp"
#include "solarium/physics/adaptive_integrator.hpp"
#include "solarium/physics/n_body_solver.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium;

    celestial::BodyRegistry registry;

    registry.initializeSolarSystem();

    auto& bodies =
        registry.bodies();

    physics::NBodySolver solver;

    physics::TimestepControllerConfig config;

    config.absoluteTolerance = 1e-8;
    config.relativeTolerance = 1e-6;

    config.minimumTimestep = 1e-6;
    config.maximumTimestep = 10.0;

    config.safetyFactor = 0.9;

    config.minimumScale = 0.2;
    config.maximumScale = 5.0;

    physics::AdaptiveIntegrator integrator(
        solver,
        config
    );

    /*
     * Save initial state.
     */
    const math::Vec3 initialPosition =
        bodies[0].position();

    const math::Vec3 initialVelocity =
        bodies[0].velocity();

    /*
     * Perform adaptive step.
     */
    integrator.step(
        bodies,
        0.01
    );

    const physics::AdaptiveStepResult& result =
        integrator.lastResult();

    /*
     * Basic sanity checks.
     */
    assert(
        std::isfinite(
            result.timestepUsed
        )
    );

    assert(
        std::isfinite(
            result.nextTimestep
        )
    );

    assert(
        std::isfinite(
            result.positionError
        )
    );

    assert(
        std::isfinite(
            result.velocityError
        )
    );

    assert(
        std::isfinite(
            result.normalizedError
        )
    );

    assert(
        result.timestepUsed > 0.0
    );

    assert(
        result.nextTimestep >=
        config.minimumTimestep
    );

    assert(
        result.nextTimestep <=
        config.maximumTimestep
    );

    /*
     * For a reasonable timestep the Solar System
     * should normally accept the step.
     */
    assert(result.accepted);

    /*
     * The state should have changed.
     */
    const double positionChange =
        (
            bodies[0].position() -
            initialPosition
        ).magnitude();

    const double velocityChange =
        (
            bodies[0].velocity() -
            initialVelocity
        ).magnitude();

    assert(
        positionChange > 0.0 ||
        velocityChange > 0.0
    );

    std::cout
        << "AdaptiveIntegratorTests passed.\n";

    std::cout
        << "Accepted: "
        << std::boolalpha
        << result.accepted
        << '\n';

    std::cout
        << "Timestep used: "
        << result.timestepUsed
        << '\n';

    std::cout
        << "Next timestep: "
        << result.nextTimestep
        << '\n';

    std::cout
        << "Position error: "
        << result.positionError
        << '\n';

    std::cout
        << "Velocity error: "
        << result.velocityError
        << '\n';

    std::cout
        << "Normalized error: "
        << result.normalizedError
        << '\n';

    return 0;
}
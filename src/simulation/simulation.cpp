#include "solarium/simulation/simulation.hpp"

#include "solarium/physics/verlet.hpp"

#include <algorithm>

namespace solarium::simulation {

Simulation::Simulation()
    : Simulation(SimulationConfig{}) {
}

Simulation::Simulation(
    const SimulationConfig& config
)
    : config_(config),
      clock_(),
      registry_(),
      solver_(),
      accumulator_(0.0) {

    initialize();
}

void Simulation::initialize() {

    registry_.initializeSolarSystem();

    accumulator_ = 0.0;

    clock_.reset();

    solver_.computeAccelerations(
        registry_.bodies()
    );

    // Store the initial accelerations.
    for (auto& body : registry_.bodies()) {
        body.setPreviousAcceleration(
            body.acceleration()
        );
    }
}

void Simulation::update(
    double realDeltaTime
) {

    if (clock_.paused()) {
        return;
    }

    const double clampedDelta =
        std::min(
            realDeltaTime,
            0.1
        );

    const double scaledDelta =
        clampedDelta *
        clock_.timeScale();

    accumulator_ += scaledDelta;

    int substeps = 0;

    while (
        accumulator_ >=
            config_.physicsStep &&
        substeps <
            config_.maxSubsteps
    ) {

        physicsStep(
            config_.physicsStep
        );

        accumulator_ -=
            config_.physicsStep;

        ++substeps;
    }

    clock_.update(
        clampedDelta
    );
}

void Simulation::physicsStep(
    double deltaTime
) {
    auto& bodies = registry_.bodies();

    if (bodies.empty()) {
        return;
    }

    // --------------------------------------------------
    // 1. Save the old acceleration
    // --------------------------------------------------

    for (auto& body : bodies) {
        body.setPreviousAcceleration(
            body.acceleration()
        );
    }

    // --------------------------------------------------
    // 2. Update positions
    // r(t+dt) = r(t) + v(t)dt + 1/2 a(t)dt²
    // --------------------------------------------------

    for (auto& body : bodies) {
        physics::VelocityVerlet::updatePosition(
            body,
            deltaTime
        );
    }

    // --------------------------------------------------
    // 3. Calculate new accelerations
    // --------------------------------------------------

    solver_.computeAccelerations(
        bodies
    );

    // --------------------------------------------------
    // 4. Update velocities
    // v(t+dt) = v(t)
    //          + 1/2 [a(t) + a(t+dt)]dt
    // --------------------------------------------------

    for (auto& body : bodies) {

        const math::Vec3 newAcceleration =
            body.acceleration();

        physics::VelocityVerlet::updateVelocity(
            body,
            newAcceleration,
            deltaTime
        );
    }
}

void Simulation::pause() {
    clock_.pause();
}

void Simulation::resume() {
    clock_.resume();
}

void Simulation::togglePause() {
    clock_.togglePause();
}

void Simulation::increaseSpeed() {
    clock_.increaseSpeed();
}

void Simulation::decreaseSpeed() {
    clock_.decreaseSpeed();
}

void Simulation::reset() {
    initialize();
}

const std::vector<
    celestial::CelestialBody
>&
Simulation::bodies() const noexcept {
    return registry_.bodies();
}

double
Simulation::simulationTime() const noexcept {
    return clock_.simulationTime();
}

double
Simulation::timeScale() const noexcept {
    return clock_.timeScale();
}

bool
Simulation::paused() const noexcept {
    return clock_.paused();
}

} // namespace solarium::simulation
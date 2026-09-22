#include "solarium/simulation/simulation.hpp"

#include <algorithm>
#include <memory>

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
      integrator_(nullptr),
      accumulator_(0.0),
      currentPhysicsStep_(
          config.physicsStep
      ) {

    initialize();
}


void Simulation::initialize() {

    registry_.initializeSolarSystem();

    accumulator_ = 0.0;

    currentPhysicsStep_ =
        config_.physicsStep;

    clock_.reset();

    solver_.computeAccelerations(
        registry_.bodies()
    );

    // Store initial accelerations.
    for (auto& body :
         registry_.bodies()) {

        body.setPreviousAcceleration(
            body.acceleration()
        );
    }

    createIntegrator();
}


void Simulation::createIntegrator() {

    switch (config_.integrator) {

        case IntegratorType::VelocityVerlet:

            integrator_ =
                std::make_unique<
                    physics::VelocityVerlet
                >(
                    solver_
                );

            break;


        case IntegratorType::RK4:

            integrator_ =
                std::make_unique<
                    physics::RK4Integrator
                >(
                    solver_
                );

            break;


        case IntegratorType::AdaptiveRK45:

            integrator_ =
                std::make_unique<
                    physics::AdaptiveIntegrator
                >(
                    solver_,
                    config_.adaptiveConfig
                );

            break;
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
            currentPhysicsStep_ &&
        substeps <
            config_.maxSubsteps
    ) {

        const bool accepted =
            physicsStep(
                currentPhysicsStep_
            );

        /*
         * Adaptive RK45 can reject a step.
         *
         * If rejected, don't consume simulation
         * time. The next iteration will retry using
         * the smaller timestep.
         */
        if (!accepted) {
            continue;
        }

        accumulator_ -=
            currentPhysicsStep_;

        ++substeps;
    }

    clock_.update(
        clampedDelta
    );
}


bool Simulation::physicsStep(
    double deltaTime
) {

    auto& bodies =
        registry_.bodies();

    if (bodies.empty()) {
        return false;
    }

    if (!integrator_) {
        return false;
    }

    integrator_->step(
        bodies,
        deltaTime
    );

    for (auto& body : bodies) {
        body.advanceRotation(deltaTime);
    }


    /*
     * Adaptive RK45 provides a new timestep
     * suggestion after every attempted step.
     */
    if (
        config_.integrator ==
        IntegratorType::AdaptiveRK45
    ) {

        auto* adaptive =
            dynamic_cast<
                physics::AdaptiveIntegrator*
            >(
                integrator_.get()
            );

        if (adaptive != nullptr) {

            const auto& result =
                adaptive->lastResult();

            currentPhysicsStep_ =
                result.nextTimestep;

            return result.accepted;
        }
    }


    /*
     * Fixed-step integrators always accept.
     */
    return true;
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

const time::AstronomicalTime&
Simulation::currentEpoch() const noexcept {

    return clock_.currentEpoch();
}

time::TimeScale
Simulation::astronomicalTimeScale() const noexcept {

    return clock_.astronomicalTimeScale();
}

} // namespace solarium::simulation
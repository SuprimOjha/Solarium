#include "solarium/simulation/simulation.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <stdexcept>

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
      ),
      ephemerisStates_{},
      ephemerisProviderId_{},
      ephemerisError_{} {

    initialize();
}


void Simulation::initialize() {

    registry_.initializeSolarSystem();

    accumulator_ = 0.0;

    currentPhysicsStep_ =
        config_.physicsStep;

    clock_.reset();

    ephemerisStates_.clear();
    ephemerisProviderId_.clear();
    ephemerisError_.clear();

    if (config_.mode != SimulationMode::NumericalSimulation &&
        !applyEphemerisStates()) {
        registry_.clear();
        createIntegrator();
        return;
    }

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

    const double clampedDelta = std::min(realDeltaTime, 0.1);

    if (config_.mode == SimulationMode::EphemerisPlayback) {
        if (!applyEphemerisStates()) {
            return;
        }
        clock_.update(clampedDelta);
        return;
    }

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

bool Simulation::applyEphemerisStates() {
    if (config_.mode == SimulationMode::EphemerisPlayback &&
        config_.ephemerisDatasets.empty()) {
        ephemerisError_ = "EPHEMERIS_PLAYBACK requires preloaded ephemeris datasets";
        clock_.pause();
        return false;
    }
    if (!config_.ephemerisProvider) {
        if (config_.ephemerisDatasets.empty()) {
            ephemerisError_ = "Ephemeris mode requires a provider or preloaded datasets";
            clock_.pause();
            return false;
        }
    }

    try {
        std::map<ephemeris::BodyId, ephemeris::CanonicalState> states;
        const auto epoch = clock_.currentEpoch().julianDate();
        for (int index = 0; index < 29; ++index) {
            const auto body = static_cast<ephemeris::BodyId>(index);
            const std::string name(ephemeris::bodyName(body));
            auto* celestialBody = registry_.find(name);
            if (celestialBody == nullptr) {
                throw std::runtime_error("simulation body is missing: " + name);
            }

            ephemeris::CanonicalState state = [&]() {
                const auto dataset = config_.ephemerisDatasets.find(body);
                if (dataset != config_.ephemerisDatasets.end() && dataset->second != nullptr) {
                    return dataset->second->stateAt(body, epoch);
                }
                if (config_.mode == SimulationMode::EphemerisPlayback) {
                    throw std::runtime_error("preloaded ephemeris dataset is missing: " + name);
                }
                if (!config_.ephemerisProvider) {
                    throw std::runtime_error("no provider for ephemeris body: " + name);
                }
                return config_.ephemerisProvider->getState(ephemeris::EphemerisRequest{
                    body, epoch, config_.ephemerisCenter,
                    config_.ephemerisFrame, config_.ephemerisTimeScale
                });
            }();
            if (state.frame() != config_.ephemerisFrame ||
                state.timeScale() != config_.ephemerisTimeScale) {
                throw std::runtime_error("provider returned state in an unexpected frame or time scale");
            }
            states.emplace(body, state);
        }

        for (const auto& [bodyId, state] : states) {
            auto* celestialBody = registry_.find(
                std::string(ephemeris::bodyName(bodyId))
            );
            celestialBody->setPosition(state.position().meters);
            celestialBody->setVelocity(state.velocity().metersPerSecond);
        }
        const std::string sourceProvider = states.empty()
            ? std::string{}
            : states.begin()->second.source().provider;
        ephemerisStates_ = std::move(states);
        ephemerisProviderId_ = sourceProvider;
        ephemerisError_.clear();
        return true;
    } catch (const std::exception& error) {
        ephemerisStates_.clear();
        ephemerisProviderId_ = config_.ephemerisProvider
            ? std::string(config_.ephemerisProvider->providerId())
            : std::string{};
        ephemerisError_ = error.what();
        clock_.pause();
        return false;
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

SimulationMode Simulation::mode() const noexcept {
    return config_.mode;
}

std::string_view Simulation::modeName() const noexcept {
    switch (config_.mode) {
    case SimulationMode::NumericalSimulation: return "NUMERICAL_SIMULATION";
    case SimulationMode::EphemerisPlayback: return "EPHEMERIS_PLAYBACK";
    case SimulationMode::Hybrid: return "HYBRID";
    }
    return "UNKNOWN";
}

DataMode Simulation::dataMode() const noexcept {
    return config_.dataMode;
}

std::string_view Simulation::dataModeName() const noexcept {
    switch (config_.dataMode) {
    case DataMode::Online: return "ONLINE";
    case DataMode::Cached: return "CACHED";
    case DataMode::Offline: return "OFFLINE";
    case DataMode::Approximate: return "APPROXIMATE";
    }
    return "UNKNOWN";
}

std::string_view Simulation::ephemerisProviderId() const noexcept {
    return ephemerisProviderId_;
}

const std::string& Simulation::ephemerisError() const noexcept {
    return ephemerisError_;
}

reference::ReferenceFrame Simulation::ephemerisFrame() const noexcept {
    return config_.ephemerisFrame;
}

ephemeris::TimeScale Simulation::ephemerisTimeScale() const noexcept {
    return config_.ephemerisTimeScale;
}

const ephemeris::CanonicalState* Simulation::ephemerisState(
    ephemeris::BodyId body
) const noexcept {
    const auto iterator = ephemerisStates_.find(body);
    return iterator == ephemerisStates_.end() ? nullptr : &iterator->second;
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
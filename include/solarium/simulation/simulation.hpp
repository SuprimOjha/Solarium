#pragma once

#include "solarium/celestial/body_registry.hpp"
#include "solarium/physics/adaptive_integrator.hpp"
#include "solarium/physics/integrator.hpp"
#include "solarium/physics/n_body_solver.hpp"
#include "solarium/physics/rk4.hpp"
#include "solarium/physics/verlet.hpp"
#include "solarium/simulation/simulation_clock.hpp"
#include "solarium/simulation/simulation_config.hpp"

#include <memory>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace solarium::simulation {

class Simulation {
public:
    Simulation();
    explicit Simulation(const SimulationConfig& config);

    void update(double realDeltaTime);

    void pause();
    void resume();
    void togglePause();

    void increaseSpeed();
    void decreaseSpeed();

    void reset();

    [[nodiscard]]
    const std::vector<celestial::CelestialBody>& bodies() const noexcept;

    [[nodiscard]]
    double simulationTime() const noexcept;

    [[nodiscard]]
    double timeScale() const noexcept;

    [[nodiscard]]
    bool paused() const noexcept;

    [[nodiscard]]
    const time::AstronomicalTime& currentEpoch() const noexcept;

    [[nodiscard]]
    time::TimeScale astronomicalTimeScale() const noexcept;

    [[nodiscard]] SimulationMode mode() const noexcept;
    [[nodiscard]] std::string_view modeName() const noexcept;
    [[nodiscard]] std::string_view ephemerisProviderId() const noexcept;
    [[nodiscard]] const std::string& ephemerisError() const noexcept;
    [[nodiscard]] reference::ReferenceFrame ephemerisFrame() const noexcept;
    [[nodiscard]] ephemeris::TimeScale ephemerisTimeScale() const noexcept;

    [[nodiscard]] const ephemeris::CanonicalState* ephemerisState(
        ephemeris::BodyId body
    ) const noexcept;

private:
    SimulationConfig config_;

    SimulationClock clock_;

    celestial::BodyRegistry registry_;

    physics::NBodySolver solver_;

    std::unique_ptr<physics::Integrator> integrator_;

    double accumulator_;

    double currentPhysicsStep_;

    std::map<ephemeris::BodyId, ephemeris::CanonicalState> ephemerisStates_;
    std::string ephemerisProviderId_;
    std::string ephemerisError_;

    void initialize();

    void createIntegrator();

    bool physicsStep(double deltaTime);
    bool applyEphemerisStates();
};

}
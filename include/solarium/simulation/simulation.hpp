#pragma once

#include "solarium/celestial/body_registry.hpp"
#include "solarium/physics/n_body_solver.hpp"
#include "solarium/simulation/simulation_clock.hpp"
#include "solarium/simulation/simulation_config.hpp"

#include <vector>

namespace solarium::simulation {

class Simulation {
public:

    Simulation();

    explicit Simulation(
        const SimulationConfig& config
    );

    void update(
        double realDeltaTime
    );

    void pause();
    void resume();
    void togglePause();

    void increaseSpeed();
    void decreaseSpeed();

    void reset();

    [[nodiscard]]
    const std::vector<
        celestial::CelestialBody
    >& bodies() const noexcept;

    [[nodiscard]]
    double simulationTime() const noexcept;

    [[nodiscard]]
    double timeScale() const noexcept;

    [[nodiscard]]
    bool paused() const noexcept;

private:

    SimulationConfig config_;

    SimulationClock clock_;

    celestial::BodyRegistry registry_;

    physics::NBodySolver solver_;

    double accumulator_;

    void initialize();

    void physicsStep(
        double deltaTime
    );
};

} // namespace solarium::simulation
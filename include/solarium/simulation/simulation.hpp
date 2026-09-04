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

private:
    SimulationConfig config_;

    SimulationClock clock_;

    celestial::BodyRegistry registry_;

    physics::NBodySolver solver_;

    std::unique_ptr<physics::Integrator> integrator_;

    double accumulator_;

    double currentPhysicsStep_;

    void initialize();

    void createIntegrator();

    bool physicsStep(double deltaTime);
};

}
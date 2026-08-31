#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/simulation/simulation_clock.hpp"

#include <vector>
#include <cstddef>

namespace solarium::simulation {

class Simulation {
public:
    Simulation();

    void update(double realDeltaTime);

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
    SimulationClock clock_;

    std::vector<
        celestial::CelestialBody
    > bodies_;

    void initialize();
    void calculateAccelerations();
    void integrate(double deltaTime);
};

}
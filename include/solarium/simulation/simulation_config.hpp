#pragma once

namespace solarium::simulation {

struct SimulationConfig {

    // Physics timestep:
    // one simulated hour.
    double physicsStep =
        3600.0;

    // Simulation speed:
    // one real second = one simulated day.
    double timeScale =
        86400.0;

    int maxSubsteps =
        100;

    bool enableTrails =
        true;
};

} // namespace solarium::simulation
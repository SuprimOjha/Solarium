#pragma once

namespace solarium::simulation {

struct SimulationConfig {

    double physicsStep = 3600.0;

    double timeScale = 86400.0;

    int maxSubsteps = 100;

    bool enableTrails = true;
};

}
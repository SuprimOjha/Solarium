#pragma once

#include "solarium/physics/timestep_controller.hpp"

namespace solarium::simulation {

enum class IntegratorType {
    VelocityVerlet,
    RK4,
    AdaptiveRK45
};

struct SimulationConfig {
    double physicsStep = 1.0;
    double timeScale = 1.0;
    int maxSubsteps = 100;

    IntegratorType integrator = IntegratorType::VelocityVerlet;

    physics::TimestepControllerConfig adaptiveConfig{};
};

}
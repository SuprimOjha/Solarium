#pragma once

#include "solarium/ephemeris/ephemeris_provider.hpp"
#include "solarium/ephemeris/ephemeris_dataset.hpp"
#include "solarium/physics/timestep_controller.hpp"
#include "solarium/simulation/simulation_mode.hpp"
#include "solarium/simulation/data_mode.hpp"

#include <memory>
#include <map>
#include <optional>

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

    SimulationMode mode = SimulationMode::NumericalSimulation;
    DataMode dataMode = DataMode::Approximate;
    std::shared_ptr<ephemeris::EphemerisProvider> ephemerisProvider;
    std::map<ephemeris::BodyId, std::shared_ptr<const ephemeris::EphemerisDataset>> ephemerisDatasets;
    reference::ReferenceFrame ephemerisFrame = reference::ReferenceFrame::Barycentric;
    std::optional<ephemeris::BodyId> ephemerisCenter;
    ephemeris::TimeScale ephemerisTimeScale = ephemeris::TimeScale::TDB;
};

}
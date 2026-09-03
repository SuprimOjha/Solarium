#pragma once

#include "solarium/physics/integrator.hpp"
#include "solarium/physics/rk45.hpp"
#include "solarium/physics/timestep_controller.hpp"

namespace solarium::physics {

struct AdaptiveStepResult {
    bool accepted = false;

    double timestepUsed = 0.0;
    double nextTimestep = 0.0;

    double positionError = 0.0;
    double velocityError = 0.0;
    double normalizedError = 0.0;
};

class AdaptiveIntegrator final : public Integrator {
public:
    AdaptiveIntegrator(
        NBodySolver& solver,
        const TimestepControllerConfig& config = {}
    );

    void step(
        std::vector<celestial::CelestialBody>& bodies,
        double deltaTime
    ) override;

    [[nodiscard]]
    const AdaptiveStepResult&
    lastResult() const noexcept;

private:
    NBodySolver& solver_;

    RK45Integrator rk45_;

    TimestepController controller_;

    AdaptiveStepResult lastResult_{};
};

} // namespace solarium::physics
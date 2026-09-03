#include "solarium/physics/timestep_controller.hpp"

#include <algorithm>
#include <cmath>

namespace solarium::physics {

TimestepController::TimestepController(
    const TimestepControllerConfig& config
)
    : config_(config) {
}

bool TimestepController::acceptStep(
    double error,
    double scale
) const noexcept {

    if (!std::isfinite(error) ||
        !std::isfinite(scale) ||
        scale <= 0.0) {
        return false;
    }

    const double tolerance =
        config_.absoluteTolerance +
        config_.relativeTolerance * scale;

    return error <= tolerance;
}


double TimestepController::nextTimestep(
    double currentTimestep,
    double error
) const noexcept {

    if (!std::isfinite(currentTimestep) ||
        currentTimestep <= 0.0) {
        return config_.minimumTimestep;
    }

    if (!std::isfinite(error) ||
        error <= 0.0) {
        return std::min(
            currentTimestep *
                config_.maximumScale,
            config_.maximumTimestep
        );
    }

    /*
     * RK45 uses a 5th-order solution.
     *
     * For adaptive stepping:
     *
     *       new_dt ≈ dt * error^(-1/5)
     *
     * The safety factor prevents the controller
     * from operating too close to the stability limit.
     */
    const double factor =
        config_.safetyFactor *
        std::pow(
            1.0 / error,
            0.2
        );

    const double clampedFactor =
        std::clamp(
            factor,
            config_.minimumScale,
            config_.maximumScale
        );

    const double newTimestep =
        currentTimestep *
        clampedFactor;

    return std::clamp(
        newTimestep,
        config_.minimumTimestep,
        config_.maximumTimestep
    );
}


const TimestepControllerConfig&
TimestepController::config() const noexcept {
    return config_;
}

} // namespace solarium::physics
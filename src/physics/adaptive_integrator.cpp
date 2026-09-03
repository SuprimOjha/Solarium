#include "solarium/physics/adaptive_integrator.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace solarium::physics {

AdaptiveIntegrator::AdaptiveIntegrator(
    NBodySolver& solver,
    const TimestepControllerConfig& config
)
    : solver_(solver),
      rk45_(solver),
      controller_(config) {
}

void AdaptiveIntegrator::step(
    std::vector<celestial::CelestialBody>& bodies,
    double deltaTime
) {
    if (deltaTime <= 0.0) {
        throw std::invalid_argument(
            "Adaptive timestep must be positive."
        );
    }

    if (bodies.empty()) {
        lastResult_ = {};
        return;
    }

    /*
     * Save the original state.
     *
     * RK45 modifies the bodies even when its error
     * estimate later tells us that the step should
     * be rejected.
     */
    const std::size_t count = bodies.size();

    std::vector<math::Vec3> originalPositions(count);
    std::vector<math::Vec3> originalVelocities(count);

    for (std::size_t i = 0; i < count; ++i) {
        originalPositions[i] =
            bodies[i].position();

        originalVelocities[i] =
            bodies[i].velocity();
    }

    /*
     * Perform RK45.
     */
    rk45_.step(
        bodies,
        deltaTime
    );

    const RK45Result& rkResult =
        rk45_.lastResult();

    /*
     * Calculate a characteristic scale for
     * position and velocity.
     *
     * This prevents position and velocity errors
     * from being compared directly because they
     * have different physical units.
     */
    double maximumScale = 0.0;

    for (std::size_t i = 0; i < count; ++i) {

        const double positionScale =
            std::max(
                originalPositions[i].magnitude(),
                bodies[i].position().magnitude()
            );

        const double velocityScale =
            std::max(
                originalVelocities[i].magnitude(),
                bodies[i].velocity().magnitude()
            );

        maximumScale =
            std::max(
                maximumScale,
                std::max(
                    positionScale,
                    velocityScale
                )
            );
    }

    /*
     * Avoid a zero scale for systems where all
     * positions and velocities are zero.
     */
    if (maximumScale <= 0.0) {
        maximumScale = 1.0;
    }

    /*
     * Combine the RK45 error into a normalized
     * dimensionless quantity.
     *
     * error / (absoluteTolerance +
     *          relativeTolerance * scale)
     */
    const TimestepControllerConfig& config =
        controller_.config();

    const double tolerance =
        config.absoluteTolerance +
        config.relativeTolerance *
            maximumScale;

    double normalizedError = 0.0;

    if (tolerance > 0.0) {
        normalizedError =
            rkResult.maximumError /
            tolerance;
    }

    /*
     * Determine whether this step is acceptable.
     */
    const bool accepted =
        controller_.acceptStep(
            normalizedError,
            1.0
        );

    /*
     * Calculate the next timestep.
     */
    const double nextTimestep =
        controller_.nextTimestep(
            deltaTime,
            normalizedError
        );

    /*
     * If rejected, restore the original state.
     */
    if (!accepted) {

        for (std::size_t i = 0; i < count; ++i) {

            bodies[i].setPosition(
                originalPositions[i]
            );

            bodies[i].setVelocity(
                originalVelocities[i]
            );
        }

        /*
         * Acceleration must correspond to the
         * restored state.
         */
        solver_.computeAccelerations(
            bodies
        );
    }

    lastResult_.accepted =
        accepted;

    lastResult_.timestepUsed =
        deltaTime;

    lastResult_.nextTimestep =
        nextTimestep;

    lastResult_.positionError =
        rkResult.positionError;

    lastResult_.velocityError =
        rkResult.velocityError;

    lastResult_.normalizedError =
        normalizedError;
}

const AdaptiveStepResult&
AdaptiveIntegrator::lastResult() const noexcept {
    return lastResult_;
}

} // namespace solarium::physics
#include "solarium/physics/rk45.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace solarium::physics {

namespace {

struct Derivative {
    math::Vec3 velocity;
    math::Vec3 acceleration;
};

using DerivativeSet = std::vector<Derivative>;

void setState(
    std::vector<celestial::CelestialBody>& bodies,
    const std::vector<math::Vec3>& positions,
    const std::vector<math::Vec3>& velocities
) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].setPosition(positions[i]);
        bodies[i].setVelocity(velocities[i]);
    }
}

DerivativeSet calculateDerivatives(
    NBodySolver& solver,
    std::vector<celestial::CelestialBody>& bodies
) {
    solver.computeAccelerations(bodies);

    DerivativeSet derivatives;
    derivatives.resize(bodies.size());

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        derivatives[i].velocity =
            bodies[i].velocity();

        derivatives[i].acceleration =
            bodies[i].acceleration();
    }

    return derivatives;
}

void applyStage(
    std::vector<celestial::CelestialBody>& bodies,
    const std::vector<math::Vec3>& originalPositions,
    const std::vector<math::Vec3>& originalVelocities,
    const DerivativeSet& k,
    double coefficient,
    double deltaTime
) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        const math::Vec3 position =
            originalPositions[i] +
            k[i].velocity *
                (coefficient * deltaTime);

        const math::Vec3 velocity =
            originalVelocities[i] +
            k[i].acceleration *
                (coefficient * deltaTime);

        bodies[i].setPosition(position);
        bodies[i].setVelocity(velocity);
    }
}

} // namespace


RK45Integrator::RK45Integrator(
    NBodySolver& solver
)
    : solver_(solver) {
}


void RK45Integrator::step(
    std::vector<celestial::CelestialBody>& bodies,
    double deltaTime
) {
    if (deltaTime <= 0.0) {
        throw std::invalid_argument(
            "RK45 timestep must be positive."
        );
    }

    if (bodies.empty()) {
        lastResult_ = {};
        return;
    }

    const std::size_t count = bodies.size();

    std::vector<math::Vec3> originalPositions(count);
    std::vector<math::Vec3> originalVelocities(count);

    for (std::size_t i = 0; i < count; ++i) {
        originalPositions[i] =
            bodies[i].position();

        originalVelocities[i] =
            bodies[i].velocity();
    }


    // ------------------------------------------------------------
    // Dormand-Prince RK45
    //
    // k1
    // ------------------------------------------------------------

    const DerivativeSet k1 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // k2
    //
    // y2 = y + h * (1/5) k1
    // ------------------------------------------------------------

    applyStage(
        bodies,
        originalPositions,
        originalVelocities,
        k1,
        1.0 / 5.0,
        deltaTime
    );

    const DerivativeSet k2 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // k3
    //
    // y3 = y + h *
    //      (3/40 k1 + 9/40 k2)
    // ------------------------------------------------------------

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 position =
            originalPositions[i] +
            (
                k1[i].velocity * (3.0 / 40.0) +
                k2[i].velocity * (9.0 / 40.0)
            ) * deltaTime;

        const math::Vec3 velocity =
            originalVelocities[i] +
            (
                k1[i].acceleration * (3.0 / 40.0) +
                k2[i].acceleration * (9.0 / 40.0)
            ) * deltaTime;

        bodies[i].setPosition(position);
        bodies[i].setVelocity(velocity);
    }

    const DerivativeSet k3 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // k4
    //
    // y4 = y + h *
    //      (44/45 k1 - 56/15 k2 + 32/9 k3)
    // ------------------------------------------------------------

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 position =
            originalPositions[i] +
            (
                k1[i].velocity * (44.0 / 45.0) +
                k2[i].velocity * (-56.0 / 15.0) +
                k3[i].velocity * (32.0 / 9.0)
            ) * deltaTime;

        const math::Vec3 velocity =
            originalVelocities[i] +
            (
                k1[i].acceleration * (44.0 / 45.0) +
                k2[i].acceleration * (-56.0 / 15.0) +
                k3[i].acceleration * (32.0 / 9.0)
            ) * deltaTime;

        bodies[i].setPosition(position);
        bodies[i].setVelocity(velocity);
    }

    const DerivativeSet k4 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // k5
    //
    // y5 = y + h *
    //      (19372/6561 k1
    //      -25360/2187 k2
    //      +64448/6561 k3
    //      -212/729 k4)
    // ------------------------------------------------------------

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 position =
            originalPositions[i] +
            (
                k1[i].velocity * (19372.0 / 6561.0) +
                k2[i].velocity * (-25360.0 / 2187.0) +
                k3[i].velocity * (64448.0 / 6561.0) +
                k4[i].velocity * (-212.0 / 729.0)
            ) * deltaTime;

        const math::Vec3 velocity =
            originalVelocities[i] +
            (
                k1[i].acceleration * (19372.0 / 6561.0) +
                k2[i].acceleration * (-25360.0 / 2187.0) +
                k3[i].acceleration * (64448.0 / 6561.0) +
                k4[i].acceleration * (-212.0 / 729.0)
            ) * deltaTime;

        bodies[i].setPosition(position);
        bodies[i].setVelocity(velocity);
    }

    const DerivativeSet k5 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // k6
    //
    // y6 = y + h *
    //      (9017/3168 k1
    //      -355/33 k2
    //      +46732/5247 k3
    //      +49/176 k4
    //      -5103/18656 k5)
    // ------------------------------------------------------------

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 position =
            originalPositions[i] +
            (
                k1[i].velocity * (9017.0 / 3168.0) +
                k2[i].velocity * (-355.0 / 33.0) +
                k3[i].velocity * (46732.0 / 5247.0) +
                k4[i].velocity * (49.0 / 176.0) +
                k5[i].velocity * (-5103.0 / 18656.0)
            ) * deltaTime;

        const math::Vec3 velocity =
            originalVelocities[i] +
            (
                k1[i].acceleration * (9017.0 / 3168.0) +
                k2[i].acceleration * (-355.0 / 33.0) +
                k3[i].acceleration * (46732.0 / 5247.0) +
                k4[i].acceleration * (49.0 / 176.0) +
                k5[i].acceleration * (-5103.0 / 18656.0)
            ) * deltaTime;

        bodies[i].setPosition(position);
        bodies[i].setVelocity(velocity);
    }

    const DerivativeSet k6 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // 5th-order solution
    //
    // y5 = y + h *
    //
    // 35/384 k1
    // + 500/1113 k3
    // + 125/192 k4
    // - 2187/6784 k5
    // + 11/84 k6
    // ------------------------------------------------------------

    std::vector<math::Vec3> fifthPositions(count);
    std::vector<math::Vec3> fifthVelocities(count);

    for (std::size_t i = 0; i < count; ++i) {

        fifthPositions[i] =
            originalPositions[i] +
            (
                k1[i].velocity * (35.0 / 384.0) +
                k3[i].velocity * (500.0 / 1113.0) +
                k4[i].velocity * (125.0 / 192.0) +
                k5[i].velocity * (-2187.0 / 6784.0) +
                k6[i].velocity * (11.0 / 84.0)
            ) * deltaTime;

        fifthVelocities[i] =
            originalVelocities[i] +
            (
                k1[i].acceleration * (35.0 / 384.0) +
                k3[i].acceleration * (500.0 / 1113.0) +
                k4[i].acceleration * (125.0 / 192.0) +
                k5[i].acceleration * (-2187.0 / 6784.0) +
                k6[i].acceleration * (11.0 / 84.0)
            ) * deltaTime;
    }


    // ------------------------------------------------------------
    // k7
    //
    // The final derivative is evaluated at the 5th-order state.
    // ------------------------------------------------------------

    setState(
        bodies,
        fifthPositions,
        fifthVelocities
    );

    const DerivativeSet k7 =
        calculateDerivatives(
            solver_,
            bodies
        );


    // ------------------------------------------------------------
    // 4th-order solution
    //
    // y4 = y + h *
    //
    // 5179/57600 k1
    // + 7571/16695 k3
    // + 393/640 k4
    // - 92097/339200 k5
    // + 187/2100 k6
    // + 1/40 k7
    // ------------------------------------------------------------

    std::vector<math::Vec3> fourthPositions(count);
    std::vector<math::Vec3> fourthVelocities(count);

    for (std::size_t i = 0; i < count; ++i) {

        fourthPositions[i] =
            originalPositions[i] +
            (
                k1[i].velocity * (5179.0 / 57600.0) +
                k3[i].velocity * (7571.0 / 16695.0) +
                k4[i].velocity * (393.0 / 640.0) +
                k5[i].velocity * (-92097.0 / 339200.0) +
                k6[i].velocity * (187.0 / 2100.0) +
                k7[i].velocity * (1.0 / 40.0)
            ) * deltaTime;

        fourthVelocities[i] =
            originalVelocities[i] +
            (
                k1[i].acceleration * (5179.0 / 57600.0) +
                k3[i].acceleration * (7571.0 / 16695.0) +
                k4[i].acceleration * (393.0 / 640.0) +
                k5[i].acceleration * (-92097.0 / 339200.0) +
                k6[i].acceleration * (187.0 / 2100.0) +
                k7[i].acceleration * (1.0 / 40.0)
            ) * deltaTime;
    }


    // ------------------------------------------------------------
    // Error estimate
    // ------------------------------------------------------------

    double maximumPositionError = 0.0;
    double maximumVelocityError = 0.0;

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 positionDifference =
            fifthPositions[i] -
            fourthPositions[i];

        const math::Vec3 velocityDifference =
            fifthVelocities[i] -
            fourthVelocities[i];

        maximumPositionError =
            std::max(
                maximumPositionError,
                positionDifference.magnitude()
            );

        maximumVelocityError =
            std::max(
                maximumVelocityError,
                velocityDifference.magnitude()
            );
    }

    lastResult_.positionError =
        maximumPositionError;

    lastResult_.velocityError =
        maximumVelocityError;

    lastResult_.maximumError =
        std::max(
            maximumPositionError,
            maximumVelocityError
        );


    // ------------------------------------------------------------
    // Keep the 5th-order solution.
    // ------------------------------------------------------------

    setState(
        bodies,
        fifthPositions,
        fifthVelocities
    );

    // Recalculate acceleration at final state.
    solver_.computeAccelerations(bodies);
}

const RK45Result&
RK45Integrator::lastResult() const noexcept {
    return lastResult_;
}

} // namespace solarium::physics
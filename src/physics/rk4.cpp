#include "solarium/physics/rk4.hpp"

#include <vector>

namespace solarium::physics {

RK4Integrator::RK4Integrator(
    NBodySolver& solver
)
    : solver_(solver) {
}

void RK4Integrator::step(
    std::vector<celestial::CelestialBody>& bodies,
    double deltaTime
) {
    const std::size_t count = bodies.size();

    if (count == 0) {
        return;
    }

    struct Derivative {
        math::Vec3 velocity;
        math::Vec3 acceleration;
    };

    std::vector<Derivative> k1(count);
    std::vector<Derivative> k2(count);
    std::vector<Derivative> k3(count);
    std::vector<Derivative> k4(count);

    std::vector<math::Vec3> originalPositions(count);
    std::vector<math::Vec3> originalVelocities(count);

    for (std::size_t i = 0; i < count; ++i) {
        originalPositions[i] =
            bodies[i].position();

        originalVelocities[i] =
            bodies[i].velocity();
    }

    /*
     * k1
     */

    solver_.computeAccelerations(bodies);

    for (std::size_t i = 0; i < count; ++i) {

        k1[i].velocity =
            originalVelocities[i];

        k1[i].acceleration =
            bodies[i].acceleration();
    }

    /*
     * k2 state
     */

    for (std::size_t i = 0; i < count; ++i) {

        bodies[i].setPosition(
            originalPositions[i] +
            k1[i].velocity *
                (0.5 * deltaTime)
        );

        bodies[i].setVelocity(
            originalVelocities[i] +
            k1[i].acceleration *
                (0.5 * deltaTime)
        );
    }

    solver_.computeAccelerations(bodies);

    for (std::size_t i = 0; i < count; ++i) {

        k2[i].velocity =
            bodies[i].velocity();

        k2[i].acceleration =
            bodies[i].acceleration();
    }

    /*
     * k3 state
     */

    for (std::size_t i = 0; i < count; ++i) {

        bodies[i].setPosition(
            originalPositions[i] +
            k2[i].velocity *
                (0.5 * deltaTime)
        );

        bodies[i].setVelocity(
            originalVelocities[i] +
            k2[i].acceleration *
                (0.5 * deltaTime)
        );
    }

    solver_.computeAccelerations(bodies);

    for (std::size_t i = 0; i < count; ++i) {

        k3[i].velocity =
            bodies[i].velocity();

        k3[i].acceleration =
            bodies[i].acceleration();
    }

    /*
     * k4 state
     */

    for (std::size_t i = 0; i < count; ++i) {

        bodies[i].setPosition(
            originalPositions[i] +
            k3[i].velocity *
                deltaTime
        );

        bodies[i].setVelocity(
            originalVelocities[i] +
            k3[i].acceleration *
                deltaTime
        );
    }

    solver_.computeAccelerations(bodies);

    for (std::size_t i = 0; i < count; ++i) {

        k4[i].velocity =
            bodies[i].velocity();

        k4[i].acceleration =
            bodies[i].acceleration();
    }

    /*
     * Final RK4 combination
     */

    constexpr double sixth = 1.0 / 6.0;

    for (std::size_t i = 0; i < count; ++i) {

        const math::Vec3 newPosition =
            originalPositions[i] +
            (
                k1[i].velocity +
                k2[i].velocity * 2.0 +
                k3[i].velocity * 2.0 +
                k4[i].velocity
            ) *
            (deltaTime * sixth);

        const math::Vec3 newVelocity =
            originalVelocities[i] +
            (
                k1[i].acceleration +
                k2[i].acceleration * 2.0 +
                k3[i].acceleration * 2.0 +
                k4[i].acceleration
            ) *
            (deltaTime * sixth);

        bodies[i].setPosition(
            newPosition
        );

        bodies[i].setVelocity(
            newVelocity
        );
    }

    /*
     * Restore the final acceleration
     * corresponding to the final state.
     */

    solver_.computeAccelerations(bodies);
}

} // namespace solarium::physics
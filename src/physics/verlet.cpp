#include "solarium/physics/verlet.hpp"

#include <stdexcept>

namespace solarium::physics {

VelocityVerlet::VelocityVerlet(
    NBodySolver& solver
)
    : solver_(solver) {
}


void VelocityVerlet::step(
    std::vector<
        celestial::CelestialBody
    >& bodies,
    double deltaTime
) {

    if (deltaTime <= 0.0) {
        throw std::invalid_argument(
            "Velocity-Verlet timestep must be positive."
        );
    }

    if (bodies.empty()) {
        return;
    }


    // --------------------------------------------------
    // 1. Save old accelerations
    // --------------------------------------------------

    for (auto& body : bodies) {

        body.setPreviousAcceleration(
            body.acceleration()
        );
    }


    // --------------------------------------------------
    // 2. Update positions
    //
    // r(t+dt) =
    // r(t) + v(t)dt + 1/2 a(t)dt²
    // --------------------------------------------------

    for (auto& body : bodies) {

        const math::Vec3 position =
            body.position();

        const math::Vec3 velocity =
            body.velocity();

        const math::Vec3 acceleration =
            body.acceleration();

        const math::Vec3 newPosition =
            position +
            velocity * deltaTime +
            acceleration *
                (0.5 * deltaTime * deltaTime);

        body.setPosition(
            newPosition
        );
    }


    // --------------------------------------------------
    // 3. Calculate new accelerations
    // --------------------------------------------------

    solver_.computeAccelerations(
        bodies
    );


    // --------------------------------------------------
    // 4. Update velocities
    //
    // v(t+dt) =
    // v(t) + 1/2[a(t)+a(t+dt)]dt
    // --------------------------------------------------

    for (auto& body : bodies) {

        const math::Vec3 oldAcceleration =
            body.previousAcceleration();

        const math::Vec3 newAcceleration =
            body.acceleration();

        const math::Vec3 velocity =
            body.velocity();

        const math::Vec3 newVelocity =
            velocity +
            (
                oldAcceleration +
                newAcceleration
            ) *
            (0.5 * deltaTime);

        body.setVelocity(
            newVelocity
        );
    }
}

} // namespace solarium::physics
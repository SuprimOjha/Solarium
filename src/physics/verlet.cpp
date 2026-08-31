#include "solarium/physics/verlet.hpp"

namespace solarium::physics {

void VelocityVerlet::integrate(
    celestial::CelestialBody& body,
    const math::Vec3& acceleration,
    double deltaTime
) {

    const math::Vec3 oldPosition =
        body.position();

    const math::Vec3 oldVelocity =
        body.velocity();

    /*
     * x(t + dt)
     *
     * x = x + v*dt + 1/2*a*dt²
     */

    const math::Vec3 newPosition =
        oldPosition +
        oldVelocity * deltaTime +
        acceleration *
            (
                0.5 *
                deltaTime *
                deltaTime
            );

    /*
     * This is the first half of the
     * Velocity-Verlet velocity update.
     *
     * The N-body solver recalculates
     * acceleration after the position
     * update.
     */

    const math::Vec3 newVelocity =
        oldVelocity +
        acceleration *
            deltaTime;

    body.setPosition(
        newPosition
    );

    body.setVelocity(
        newVelocity
    );
}

} // namespace solarium::physics
#include "solarium/physics/verlet.hpp"

namespace solarium::physics {

void VelocityVerlet::integrate(
    celestial::CelestialBody& body,
    const math::Vec3& newAcceleration,
    double dt
) {
    const math::Vec3 oldAcceleration =
        body.acceleration();

    const math::Vec3 newPosition =
        body.position() +
        body.velocity() * dt +
        oldAcceleration * (0.5 * dt * dt);

    const math::Vec3 newVelocity =
        body.velocity() +
        (oldAcceleration + newAcceleration) *
        (0.5 * dt);

    body.setPosition(newPosition);
    body.setVelocity(newVelocity);
    body.setAcceleration(newAcceleration);
}

} // namespace solarium::physics
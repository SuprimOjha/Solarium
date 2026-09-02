#include "solarium/physics/verlet.hpp"

namespace solarium::physics {

void VelocityVerlet::updatePosition(
    celestial::CelestialBody& body,
    double deltaTime
) {
    const math::Vec3 position = body.position();
    const math::Vec3 velocity = body.velocity();
    const math::Vec3 acceleration = body.acceleration();

    const math::Vec3 newPosition =
        position +
        velocity * deltaTime +
        acceleration * (0.5 * deltaTime * deltaTime);

    body.setPosition(newPosition);
}

void VelocityVerlet::updateVelocity(
    celestial::CelestialBody& body,
    const math::Vec3& newAcceleration,
    double deltaTime
) {
    const math::Vec3 velocity = body.velocity();
    const math::Vec3 oldAcceleration = body.previousAcceleration();

    const math::Vec3 newVelocity =
        velocity +
        (oldAcceleration + newAcceleration) * (0.5 * deltaTime);

    body.setVelocity(newVelocity);
    body.setPreviousAcceleration(body.acceleration());
    body.setAcceleration(newAcceleration);
}

} // namespace solarium::physics
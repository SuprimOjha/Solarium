#include "solarium/physics/gravity.hpp"

namespace solarium::physics {

math::Vec3 gravitationalAcceleration(
    const math::Vec3& sourcePosition,
    double sourceMass,
    const math::Vec3& targetPosition
) {
    const math::Vec3 displacement =
        sourcePosition - targetPosition;

    const double distanceSquared =
        displacement.magnitudeSquared();

    if (distanceSquared == 0.0) {
        return {};
    }

    const double distance =
        displacement.magnitude();

    const double scale =
        GravitationalConstant *
        sourceMass /
        (distanceSquared * distance);

    return displacement * scale;
}

math::Vec3 gravitationalAcceleration(
    const celestial::CelestialBody& source,
    const celestial::CelestialBody& target
) {
    return gravitationalAcceleration(
        source.position(),
        source.mass(),
        target.position()
    );
}

} // namespace solarium::physics
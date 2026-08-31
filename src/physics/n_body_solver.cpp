#include "solarium/physics/n_body_solver.hpp"

#include "solarium/math/constants.hpp"

#include <cmath>
#include <cstddef>

namespace solarium::physics {

math::Vec3
NBodySolver::accelerationFromBody(
    const celestial::CelestialBody& source,
    const celestial::CelestialBody& target
) {

    const math::Vec3 direction =
        source.position() -
        target.position();

    const double distanceSquared =
        direction.magnitudeSquared();

    constexpr double MinimumDistance =
        1.0e6;

    constexpr double MinimumDistanceSquared =
        MinimumDistance *
        MinimumDistance;

    if (
        distanceSquared <
        MinimumDistanceSquared
    ) {
        return {};
    }

    const double distance =
        std::sqrt(distanceSquared);

    /*
     * a = G * M / r^3 * r_vector
     */

    const double inverseCube =
        1.0 /
        (
            distanceSquared *
            distance
        );

    const double magnitude =
        math::GravitationalConstant *
        source.mass() *
        inverseCube;

    return direction * magnitude;
}

void NBodySolver::computeAccelerations(
    std::vector<
        celestial::CelestialBody
    >& bodies
) const {

    const std::size_t count =
        bodies.size();

    for (
        std::size_t i = 0;
        i < count;
        ++i
    ) {

        math::Vec3 acceleration{};

        for (
            std::size_t j = 0;
            j < count;
            ++j
        ) {

            if (i == j) {
                continue;
            }

            acceleration +=
                accelerationFromBody(
                    bodies[j],
                    bodies[i]
                );
        }

        bodies[i].setAcceleration(
            acceleration
        );
    }
}

} // namespace solarium::physics
#include "solarium/physics/physics_diagnostics.hpp"

#include "solarium/math/constants.hpp"

#include <cmath>
#include <cstddef>

namespace solarium::physics {

PhysicsState PhysicsDiagnostics::calculate(
    const std::vector<
        celestial::CelestialBody
    >& bodies
) {

    PhysicsState state{
        0.0,
        0.0,
        0.0,
        {},
        {}
    };

    // -----------------------------------------
    // Kinetic energy
    // -----------------------------------------

    for (const auto& body : bodies) {

        const double velocitySquared =
            body.velocity().magnitudeSquared();

        state.kineticEnergy +=
            0.5 *
            body.mass() *
            velocitySquared;
    }

    // -----------------------------------------
    // Linear momentum
    // -----------------------------------------

    for (const auto& body : bodies) {

        state.linearMomentum +=
            body.velocity() *
            body.mass();
    }

    // -----------------------------------------
    // Angular momentum
    // -----------------------------------------

    for (const auto& body : bodies) {

        const math::Vec3 momentum =
            body.velocity() *
            body.mass();

        state.angularMomentum +=
            body.position().cross(
                momentum
            );
    }

    // -----------------------------------------
    // Gravitational potential energy
    // -----------------------------------------

    const std::size_t count =
        bodies.size();

    for (
        std::size_t i = 0;
        i < count;
        ++i
    ) {

        for (
            std::size_t j = i + 1;
            j < count;
            ++j
        ) {

            const math::Vec3 separation =
                bodies[j].position() -
                bodies[i].position();

            const double distance =
                separation.magnitude();

            if (distance == 0.0) {
                continue;
            }

            state.potentialEnergy -=
                math::GravitationalConstant *
                bodies[i].mass() *
                bodies[j].mass() /
                distance;
        }
    }

    state.totalEnergy =
        state.kineticEnergy +
        state.potentialEnergy;

    return state;
}

double PhysicsDiagnostics::relativeEnergyError(
    double initialEnergy,
    double currentEnergy
) noexcept {

    if (initialEnergy == 0.0) {
        return 0.0;
    }

    return std::abs(
        (
            currentEnergy -
            initialEnergy
        ) /
        initialEnergy
    );
}

} // namespace solarium::physics
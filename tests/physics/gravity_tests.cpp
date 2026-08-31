#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/gravity.hpp"

#include <cassert>
#include <cmath>

int main() {

    using solarium::celestial::CelestialBody;
    using solarium::math::Vec3;
    using solarium::physics::gravitationalAcceleration;

    constexpr double sunMass = 1.98847e30;
    constexpr double distance = 1.495978707e11;

    CelestialBody sun(
        "Sun",
        sunMass,
        1.0,
        Vec3{0.0, 0.0, 0.0},
        Vec3{}
    );

    CelestialBody earth(
        "Earth",
        5.9722e24,
        1.0,
        Vec3{distance, 0.0, 0.0},
        Vec3{}
    );

    const Vec3 acceleration =
        gravitationalAcceleration(sun, earth);

    // Expected solar gravitational acceleration
    // at Earth's orbital distance is approximately
    // 0.00593 m/s^2.
    const double expected = 0.00593;

    assert(
        std::abs(acceleration.x + expected)
        < 1e-5
    );

    assert(
        std::abs(acceleration.y)
        < 1e-12
    );

    assert(
        std::abs(acceleration.z)
        < 1e-12
    );

    return 0;
}
#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/verlet.hpp"

#include <cassert>
#include <cmath>

int main() {

    using solarium::celestial::CelestialBody;
    using solarium::math::Vec3;
    using solarium::physics::VelocityVerlet;

    CelestialBody body(
        "TestBody",
        1.0,
        1.0,
        Vec3{0.0, 0.0, 0.0},
        Vec3{10.0, 0.0, 0.0}
    );

    body.setAcceleration(
        Vec3{2.0, 0.0, 0.0}
    );

    constexpr double dt = 1.0;

    // Constant acceleration = 2 m/s².
    //
    // x = x0 + v0*t + 0.5*a*t²
    //
    // x = 0 + 10 + 1 = 11

    const Vec3 newAcceleration{
        2.0,
        0.0,
        0.0
    };

    VelocityVerlet::integrate(
        body,
        newAcceleration,
        dt
    );

    assert(
        std::abs(body.position().x - 11.0)
        < 1e-10
    );

    // v = v0 + a*t
    // v = 10 + 2 = 12

    assert(
        std::abs(body.velocity().x - 12.0)
        < 1e-10
    );

    return 0;
}
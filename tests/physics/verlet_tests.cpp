#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/verlet.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    using namespace solarium;

    celestial::CelestialBody body(
        "Test",
        1.0,
        1.0,
        math::Vec3(0.0, 0.0, 0.0),
        math::Vec3(1.0, 0.0, 0.0)
    );

    // Initial acceleration.
    body.setAcceleration(
        math::Vec3(0.0, 0.0, 0.0)
    );

    body.setPreviousAcceleration(
        body.acceleration()
    );

    constexpr double deltaTime = 1.0;

    // Velocity-Verlet position update.
    physics::VelocityVerlet::updatePosition(
        body,
        deltaTime
    );

    // With v = 1 and a = 0:
    // x = x0 + v*dt = 1
    assert(
        std::abs(body.position().x - 1.0) < 1e-12
    );

    // New acceleration.
    const math::Vec3 newAcceleration(
        0.0,
        1.0,
        0.0
    );

    physics::VelocityVerlet::updateVelocity(
        body,
        newAcceleration,
        deltaTime
    );

    // v_new = v_old + 0.5*(a_old + a_new)*dt
    //
    // x velocity:
    // 1 + 0.5*(0 + 0)*1 = 1
    //
    // y velocity:
    // 0 + 0.5*(0 + 1)*1 = 0.5

    assert(
        std::abs(body.velocity().x - 1.0) < 1e-12
    );

    assert(
        std::abs(body.velocity().y - 0.5) < 1e-12
    );

    std::cout << "VerletTests passed.\n";

    return 0;
}
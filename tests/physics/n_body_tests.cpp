#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/n_body_solver.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

int main() {

    using namespace solarium;

    celestial::CelestialBody sun(
        "Sun",
        1.98847e30,
        6.9634e8,

        math::Vec3{
            0.0,
            0.0,
            0.0
        },

        math::Vec3{
            0.0,
            0.0,
            0.0
        }
    );

    celestial::CelestialBody earth(
        "Earth",
        5.9722e24,
        6.371e6,

        math::Vec3{
            1.495978707e11,
            0.0,
            0.0
        },

        math::Vec3{
            0.0,
            29'780.0,
            0.0
        }
    );

    std::vector<
        celestial::CelestialBody
    > bodies;

    bodies.push_back(sun);
    bodies.push_back(earth);

    physics::NBodySolver solver;

    solver.computeAccelerations(
        bodies
    );

    const math::Vec3 acceleration =
        bodies[1].acceleration();

    // Earth's acceleration should point
    // toward the Sun.
    assert(
        acceleration.x < 0.0
    );

    assert(
        std::abs(acceleration.y)
        < 1.0e-20
    );

    assert(
        std::abs(acceleration.z)
        < 1.0e-20
    );

    // Expected solar acceleration at 1 AU.
    constexpr double expected =
        0.00593;

    assert(
        std::abs(
            std::abs(acceleration.x)
            - expected
        ) < 0.0001
    );

    std::cout
        << "NBodyTests passed.\n";

    return 0;
}
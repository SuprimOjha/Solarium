#include "solarium/physics/rk4.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    using namespace solarium;

    /*
     * This test is intentionally simple.
     *
     * Initial:
     * position = 0
     * velocity = 0
     *
     * Constant acceleration:
     * a = 1
     *
     * After t = 1:
     *
     * position = 0.5
     * velocity = 1
     */

    constexpr double tolerance = 1e-10;

    celestial::CelestialBody body(
        "Test",
        1.0,
        1.0,
        math::Vec3(0.0, 0.0, 0.0),
        math::Vec3(0.0, 0.0, 0.0)
    );

    body.setAcceleration(
        math::Vec3(1.0, 0.0, 0.0)
    );

    /*
     * This test will later use a deterministic
     * test solver rather than the real N-body solver.
     */

    assert(
        std::abs(
            body.acceleration().x - 1.0
        ) < tolerance
    );

    std::cout
        << "RK4Tests basic state passed.\n";

    return 0;
}
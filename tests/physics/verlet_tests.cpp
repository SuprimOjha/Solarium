#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/n_body_solver.hpp"
#include "solarium/physics/verlet.hpp"

#include <cmath>
#include <iostream>
#include <vector>

namespace {

bool nearlyEqual(
    double a,
    double b,
    double tolerance = 1e-10)
{
    return std::abs(a - b) <= tolerance;
}

bool nearlyEqual(
    const solarium::math::Vec3& a,
    const solarium::math::Vec3& b,
    double tolerance = 1e-10)
{
    return nearlyEqual(a.x, b.x, tolerance) &&
           nearlyEqual(a.y, b.y, tolerance) &&
           nearlyEqual(a.z, b.z, tolerance);
}

void printVec(
    const char* name,
    const solarium::math::Vec3& value)
{
    std::cout
        << name
        << " = ("
        << value.x << ", "
        << value.y << ", "
        << value.z << ")\n";
}

} // namespace

int main()
{
    using namespace solarium;

    std::cout << "Running Velocity-Verlet tests...\n";

    // ============================================================
    // TEST 1: Single body with constant velocity
    // ============================================================
    {
        celestial::CelestialBody body(
            "TestBody",
            1.0,
            1.0,
            math::Vec3{0.0, 0.0, 0.0},
            math::Vec3{1.0, 0.0, 0.0}
        );

        std::vector<celestial::CelestialBody> bodies;
        bodies.push_back(body);

        physics::NBodySolver solver;
        physics::VelocityVerlet verlet(solver);

        const double dt = 1.0;

        verlet.step(bodies, dt);

        const auto& result = bodies[0];

        const math::Vec3 expectedPosition{
            1.0,
            0.0,
            0.0
        };

        const math::Vec3 expectedVelocity{
            1.0,
            0.0,
            0.0
        };

        if (!nearlyEqual(
                result.position(),
                expectedPosition))
        {
            std::cerr
                << "FAILED: constant velocity position\n";

            printVec("Expected", expectedPosition);
            printVec("Actual", result.position());

            return 1;
        }

        if (!nearlyEqual(
                result.velocity(),
                expectedVelocity))
        {
            std::cerr
                << "FAILED: constant velocity\n";

            printVec("Expected", expectedVelocity);
            printVec("Actual", result.velocity());

            return 1;
        }
    }

    // ============================================================
    // TEST 2: Two-body gravitational interaction
    //
    // IMPORTANT:
    // NBodySolver ignores distances below 1e6.
    // Therefore the bodies must be farther apart than 1e6.
    // ============================================================
    {
        constexpr double distance = 2.0e6;

        celestial::CelestialBody bodyA(
            "BodyA",
            1.0e30,
            1.0,
            math::Vec3{
                -distance / 2.0,
                0.0,
                0.0
            },
            math::Vec3{
                0.0,
                0.0,
                0.0
            }
        );

        celestial::CelestialBody bodyB(
            "BodyB",
            1.0e30,
            1.0,
            math::Vec3{
                distance / 2.0,
                0.0,
                0.0
            },
            math::Vec3{
                0.0,
                0.0,
                0.0
            }
        );

        std::vector<celestial::CelestialBody> bodies;
        bodies.push_back(bodyA);
        bodies.push_back(bodyB);

        physics::NBodySolver solver;

        // Calculate initial accelerations.
        solver.computeAccelerations(bodies);

        const math::Vec3 initialPositionA =
            bodies[0].position();

        const math::Vec3 initialPositionB =
            bodies[1].position();

        const math::Vec3 initialAccelerationA =
            bodies[0].acceleration();

        const math::Vec3 initialAccelerationB =
            bodies[1].acceleration();

        // --------------------------------------------------------
        // Body A is on the left.
        // Therefore its acceleration must point +X.
        // --------------------------------------------------------
        if (initialAccelerationA.x <= 0.0)
        {
            std::cerr
                << "FAILED: BodyA acceleration should point "
                   "toward BodyB\n";

            printVec(
                "BodyA acceleration",
                initialAccelerationA);

            return 1;
        }

        // --------------------------------------------------------
        // Body B is on the right.
        // Therefore its acceleration must point -X.
        // --------------------------------------------------------
        if (initialAccelerationB.x >= 0.0)
        {
            std::cerr
                << "FAILED: BodyB acceleration should point "
                   "toward BodyA\n";

            printVec(
                "BodyB acceleration",
                initialAccelerationB);

            return 1;
        }

        // --------------------------------------------------------
        // Perform one Velocity-Verlet step.
        // --------------------------------------------------------
        physics::VelocityVerlet verlet(solver);

        const double dt = 1.0;

        verlet.step(bodies, dt);

        // --------------------------------------------------------
        // Body A must move toward Body B.
        // --------------------------------------------------------
        if (bodies[0].position().x <= initialPositionA.x)
        {
            std::cerr
                << "FAILED: BodyA did not move toward BodyB\n";

            return 1;
        }

        // --------------------------------------------------------
        // Body B must move toward Body A.
        // --------------------------------------------------------
        if (bodies[1].position().x >= initialPositionB.x)
        {
            std::cerr
                << "FAILED: BodyB did not move toward BodyA\n";

            return 1;
        }

        // --------------------------------------------------------
        // Body A velocity must point toward Body B.
        // --------------------------------------------------------
        if (bodies[0].velocity().x <= 0.0)
        {
            std::cerr
                << "FAILED: BodyA velocity direction\n";

            printVec(
                "BodyA velocity",
                bodies[0].velocity());

            return 1;
        }

        // --------------------------------------------------------
        // Body B velocity must point toward Body A.
        // --------------------------------------------------------
        if (bodies[1].velocity().x >= 0.0)
        {
            std::cerr
                << "FAILED: BodyB velocity direction\n";

            printVec(
                "BodyB velocity",
                bodies[1].velocity());

            return 1;
        }
    }

    // ============================================================
    // TEST 3: Zero-acceleration / free-body motion
    // ============================================================
    {
        celestial::CelestialBody body(
            "FreeBody",
            1.0,
            1.0,
            math::Vec3{
                5.0,
                -3.0,
                2.0
            },
            math::Vec3{
                2.0,
                4.0,
                -1.0
            }
        );

        std::vector<celestial::CelestialBody> bodies;
        bodies.push_back(body);

        physics::NBodySolver solver;
        physics::VelocityVerlet verlet(solver);

        const double dt = 0.5;

        verlet.step(bodies, dt);

        const math::Vec3 expectedPosition{
            6.0,
            -1.0,
            1.5
        };

        const math::Vec3 expectedVelocity{
            2.0,
            4.0,
            -1.0
        };

        if (!nearlyEqual(
                bodies[0].position(),
                expectedPosition))
        {
            std::cerr
                << "FAILED: free-body position\n";

            printVec(
                "Expected",
                expectedPosition);

            printVec(
                "Actual",
                bodies[0].position());

            return 1;
        }

        if (!nearlyEqual(
                bodies[0].velocity(),
                expectedVelocity))
        {
            std::cerr
                << "FAILED: free-body velocity\n";

            printVec(
                "Expected",
                expectedVelocity);

            printVec(
                "Actual",
                bodies[0].velocity());

            return 1;
        }
    }

    std::cout
        << "Velocity-Verlet tests passed.\n";

    return 0;
}
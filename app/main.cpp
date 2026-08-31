#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/gravity.hpp"
#include "solarium/physics/verlet.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

constexpr double SolarMass = 1.98847e30;
constexpr double SolarRadius = 6.9634e8;

constexpr double EarthMass = 5.9722e24;
constexpr double EarthRadius = 6.371e6;

constexpr double AstronomicalUnit = 1.495978707e11;

constexpr double EarthOrbitalVelocity =
    29'780.0;

constexpr double Days =
    86'400.0;

}

int main() {

    using solarium::celestial::CelestialBody;
    using solarium::math::Vec3;
    using solarium::physics::VelocityVerlet;
    using solarium::physics::gravitationalAcceleration;

    CelestialBody sun(
        "Sun",
        SolarMass,
        SolarRadius,
        Vec3{0.0, 0.0, 0.0},
        Vec3{0.0, 0.0, 0.0}
    );

    CelestialBody earth(
        "Earth",
        EarthMass,
        EarthRadius,
        Vec3{AstronomicalUnit, 0.0, 0.0},
        Vec3{0.0, EarthOrbitalVelocity, 0.0}
    );

    // Initial acceleration.
    const Vec3 initialAcceleration =
        gravitationalAcceleration(sun, earth);

    earth.setAcceleration(initialAcceleration);

    // One hour timestep.
    constexpr double dt = 3'600.0;

    // Simulate one year.
    constexpr int steps =
        static_cast<int>(365.25 * Days / dt);

    std::cout << std::fixed
              << std::setprecision(3);

    std::cout << "=====================================\n";
    std::cout << "          SOLARIUM V1\n";
    std::cout << "     Celestial Mechanics Engine\n";
    std::cout << "=====================================\n\n";

    std::cout << "Simulating Earth orbit...\n\n";

    for (int step = 0; step < steps; ++step) {

        const Vec3 acceleration =
            gravitationalAcceleration(sun, earth);

        VelocityVerlet::integrate(
            earth,
            acceleration,
            dt
        );

        // Print approximately every 30 days.
        if (step % (24 * 30) == 0) {

            const double distance =
                earth.position().magnitude();

            const double daysElapsed =
                step * dt / Days;

            std::cout
                << "Day: "
                << std::setw(6)
                << daysElapsed
                << " | Distance from Sun: "
                << distance / AstronomicalUnit
                << " AU\n";
        }
    }

    std::cout << "\nSimulation complete.\n";

    const double finalDistance =
        earth.position().magnitude();

    std::cout
        << "Final Earth distance: "
        << finalDistance / AstronomicalUnit
        << " AU\n";

    return 0;
}
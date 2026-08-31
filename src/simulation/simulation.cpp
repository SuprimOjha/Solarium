#include "solarium/simulation/simulation.hpp"

#include "solarium/physics/gravity.hpp"
#include "solarium/physics/verlet.hpp"

namespace solarium::simulation {

namespace {

constexpr double AstronomicalUnit =
    1.495978707e11;

constexpr double SunMass =
    1.98847e30;

constexpr double SunRadius =
    6.9634e8;

constexpr double EarthMass =
    5.9722e24;

constexpr double EarthRadius =
    6.371e6;

constexpr double EarthOrbitalVelocity =
    29'780.0;

}

Simulation::Simulation()
    : clock_(),
      bodies_() {

    initialize();
}

void Simulation::initialize() {

    bodies_.clear();

    celestial::CelestialBody sun(
        "Sun",
        SunMass,
        SunRadius,
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
        EarthMass,
        EarthRadius,
        math::Vec3{
            AstronomicalUnit,
            0.0,
            0.0
        },
        math::Vec3{
            0.0,
            EarthOrbitalVelocity,
            0.0
        }
    );

    earth.setAcceleration(
        physics::gravitationalAcceleration(
            sun,
            earth
        )
    );

    bodies_.push_back(sun);
    bodies_.push_back(earth);
}

void Simulation::update(
    double realDeltaTime
) {
    clock_.update(realDeltaTime);

    if (clock_.paused()) {
        return;
    }

    const double deltaTime =
        clock_.deltaTime();

    if (deltaTime <= 0.0) {
        return;
    }

    calculateAccelerations();

    integrate(deltaTime);
}

void Simulation::calculateAccelerations() {

    if (bodies_.size() < 2) {
        return;
    }

    for (
        std::size_t i = 1;
        i < bodies_.size();
        ++i
    ) {

        const math::Vec3 acceleration =
            physics::gravitationalAcceleration(
                bodies_[0],
                bodies_[i]
            );

        bodies_[i].setAcceleration(
            acceleration
        );
    }
}

void Simulation::integrate(
    double deltaTime
) {

    if (bodies_.size() < 2) {
        return;
    }

    physics::VelocityVerlet::integrate(
        bodies_[1],
        bodies_[1].acceleration(),
        deltaTime
    );
}

void Simulation::pause() {
    clock_.pause();
}

void Simulation::resume() {
    clock_.resume();
}

void Simulation::togglePause() {
    clock_.togglePause();
}

void Simulation::increaseSpeed() {
    clock_.increaseSpeed();
}

void Simulation::decreaseSpeed() {
    clock_.decreaseSpeed();
}

void Simulation::reset() {

    clock_.reset();
    initialize();
}

const std::vector<
    celestial::CelestialBody
>& Simulation::bodies()
    const noexcept {

    return bodies_;
}

double Simulation::simulationTime()
    const noexcept {

    return clock_.simulationTime();
}

double Simulation::timeScale()
    const noexcept {

    return clock_.timeScale();
}

bool Simulation::paused()
    const noexcept {

    return clock_.paused();
}

}
#include "solarium/simulation/simulation.hpp"
#include "solarium/simulation/simulation_clock.hpp"

#include <cassert>
#include <iostream>

int main() {

    using namespace solarium;

    simulation::Simulation simulation;

    const auto& bodies =
        simulation.bodies();

    assert(
        bodies.size() == 29
    );

    assert(
        bodies[0].name() == "Sun"
    );

    assert(
        bodies[1].name() == "Mercury"
    );

    assert(
        bodies[2].name() == "Venus"
    );

    assert(
        bodies[3].name() == "Earth"
    );

    assert(
        bodies[4].name() == "Mars"
    );

    assert(
        bodies[5].name() == "Jupiter"
    );

    assert(
        bodies[6].name() == "Saturn"
    );

    assert(
        bodies[7].name() == "Uranus"
    );

    assert(
        bodies[8].name() == "Neptune"
    );

    assert(bodies[3].radius() == 6.371e6);
    assert(bodies[9].radius() == 1.7374e6);

    assert(bodies[9].name() == "Moon");
    assert(bodies[9].parentName() == "Earth");
    assert(bodies[12].parentName() == "Jupiter");
    assert(bodies[23].parentName() == "Uranus");
    assert(bodies[28].parentName() == "Neptune");

    assert(
        simulation.simulationTime() == 0.0
    );

    assert(
        !simulation.paused()
    );

    assert(simulation.currentEpoch().scale() == time::TimeScale::TDB);
    const double initialEpoch = simulation.currentEpoch().julianDay();
    simulation.update(0.001);
    assert(simulation.currentEpoch().julianDay() > initialEpoch);

    simulation.pause();
    const double pausedEpoch = simulation.currentEpoch().julianDay();
    simulation.update(0.1);
    assert(simulation.currentEpoch().julianDay() == pausedEpoch);

    simulation::SimulationClock clock;
    clock.pause();
    const double clockEpoch = clock.currentEpoch().julianDay();
    clock.step(1.0);
    assert(clock.currentEpoch().julianDay() > clockEpoch);
    assert(clock.paused());

    std::cout
        << "SimulationTests passed.\n";

    return 0;
}
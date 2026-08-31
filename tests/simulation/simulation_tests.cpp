#include "solarium/simulation/simulation.hpp"

#include <cassert>
#include <iostream>

int main() {

    using namespace solarium;

    simulation::Simulation simulation;

    const auto& bodies =
        simulation.bodies();

    assert(
        bodies.size() == 9
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

    assert(
        simulation.simulationTime() == 0.0
    );

    assert(
        !simulation.paused()
    );

    std::cout
        << "SimulationTests passed.\n";

    return 0;
}
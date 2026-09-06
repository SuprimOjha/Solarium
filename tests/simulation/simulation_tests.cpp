#include "solarium/simulation/simulation.hpp"

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

    std::cout
        << "SimulationTests passed.\n";

    return 0;
}
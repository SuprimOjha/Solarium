#include "solarium/physics/timestep_controller.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium::physics;

    TimestepControllerConfig config;

    config.absoluteTolerance = 1e-6;
    config.relativeTolerance = 1e-6;

    config.minimumTimestep = 0.001;
    config.maximumTimestep = 10.0;

    config.safetyFactor = 0.9;

    config.minimumScale = 0.2;
    config.maximumScale = 5.0;

    TimestepController controller(config);


    // ------------------------------------------------------------
    // Small error should be accepted.
    // ------------------------------------------------------------

    const bool smallErrorAccepted =
        controller.acceptStep(
            1e-10,
            1.0
        );

    assert(smallErrorAccepted);


    // ------------------------------------------------------------
    // Large error should be rejected.
    // ------------------------------------------------------------

    const bool largeErrorAccepted =
        controller.acceptStep(
            1.0,
            1.0
        );

    assert(!largeErrorAccepted);


    // ------------------------------------------------------------
    // Small error should allow a larger timestep.
    // ------------------------------------------------------------

    const double currentTimestep = 1.0;

    const double largerTimestep =
        controller.nextTimestep(
            currentTimestep,
            1e-10
        );

    assert(
        largerTimestep > currentTimestep
    );

    assert(
        largerTimestep <=
        config.maximumTimestep
    );


    // ------------------------------------------------------------
    // Large error should reduce timestep.
    // ------------------------------------------------------------

    const double smallerTimestep =
        controller.nextTimestep(
            currentTimestep,
            100.0
        );

    assert(
        smallerTimestep < currentTimestep
    );


    // ------------------------------------------------------------
    // Timestep must respect minimum bound.
    // ------------------------------------------------------------

    const double minimum =
        controller.nextTimestep(
            0.000001,
            1e100
        );

    assert(
        minimum >=
        config.minimumTimestep
    );


    // ------------------------------------------------------------
    // Timestep must respect maximum bound.
    // ------------------------------------------------------------

    const double maximum =
        controller.nextTimestep(
            9.0,
            1e-20
        );

    assert(
        maximum <=
        config.maximumTimestep
    );


    // ------------------------------------------------------------
    // Invalid error should be handled.
    // ------------------------------------------------------------

    const double invalidError =
        controller.nextTimestep(
            1.0,
            -1.0
        );

    assert(
        std::isfinite(invalidError)
    );


    std::cout
        << "TimestepControllerTests passed.\n";

    return 0;
}
#include "solarium/orbital/kepler_solver.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {

    constexpr double eccentricity = 0.1;

    constexpr double eccentricAnomaly =
        1.0;

    const double meanAnomaly =
        eccentricAnomaly -
        eccentricity *
            std::sin(eccentricAnomaly);

    const double solved =
        solarium::orbital::KeplerSolver::
            solveEccentricAnomaly(
                meanAnomaly,
                eccentricity
            );

    assert(
        std::abs(
            solved -
            eccentricAnomaly
        ) < 1e-12
    );

    std::cout
        << "KeplerTests passed.\n";

    return 0;
}
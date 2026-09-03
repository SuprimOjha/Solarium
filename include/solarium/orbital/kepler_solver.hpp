#pragma once

namespace solarium::orbital {

class KeplerSolver {
public:

    [[nodiscard]]
    static double solveEccentricAnomaly(
        double meanAnomaly,
        double eccentricity,
        int maxIterations = 20,
        double tolerance = 1e-12
    );
};

} // namespace solarium::orbital
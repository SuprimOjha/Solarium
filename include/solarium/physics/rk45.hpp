#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/integrator.hpp"
#include "solarium/physics/n_body_solver.hpp"

#include <vector>

namespace solarium::physics {

struct RK45Result {
    double positionError = 0.0;
    double velocityError = 0.0;
    double maximumError = 0.0;
};

class RK45Integrator final : public Integrator {
public:
    explicit RK45Integrator(NBodySolver& solver);

    void step(
        std::vector<celestial::CelestialBody>& bodies,
        double deltaTime
    ) override;

    [[nodiscard]]
    const RK45Result& lastResult() const noexcept;

private:
    NBodySolver& solver_;
    RK45Result lastResult_{};
};

} // namespace solarium::physics
#pragma once

#include "solarium/physics/integrator.hpp"
#include "solarium/physics/n_body_solver.hpp"

namespace solarium::physics {

class VelocityVerlet final : public Integrator {
public:

    explicit VelocityVerlet(
        NBodySolver& solver
    );

    void step(
        std::vector<
            celestial::CelestialBody
        >& bodies,
        double deltaTime
    ) override;

private:

    NBodySolver& solver_;
};

} // namespace solarium::physics
#pragma once

#include "solarium/celestial/celestial_body.hpp"
namespace solarium::physics {
class VelocityVerlet {
    public:
    static void updatePosition(
        celestial::CelestialBody& body,
        double deltaTime
    );
    static void updateVelocity(
        celestial::CelestialBody& body,
        const math::Vec3& newAcceleration,
        double deltaTime
    );
};
}
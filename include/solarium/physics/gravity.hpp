#pragma once

#include "solarium/math/vec3.hpp"
#include "solarium/celestial/celestial_body.hpp"

namespace solarium::physics {

inline constexpr double GravitationalConstant =
    6.67430e-11;

[[nodiscard]]
math::Vec3 gravitationalAcceleration(
    const celestial::CelestialBody& source,
    const celestial::CelestialBody& target
);

[[nodiscard]]
math::Vec3 gravitationalAcceleration(
    const math::Vec3& sourcePosition,
    double sourceMass,
    const math::Vec3& targetPosition
);

} // namespace solarium::physics
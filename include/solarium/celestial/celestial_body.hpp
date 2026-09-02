#pragma once

#include "solarium/math/vec3.hpp"

#include <string>

namespace solarium::celestial {

class CelestialBody {
public:

    CelestialBody(
        std::string name,
        double mass,
        double radius,
        const math::Vec3& position,
        const math::Vec3& velocity
    );

    [[nodiscard]]
    const std::string& name() const noexcept;

    [[nodiscard]]
    double mass() const noexcept;

    [[nodiscard]]
    double radius() const noexcept;

    [[nodiscard]]
    const math::Vec3& position() const noexcept;

    [[nodiscard]]
    const math::Vec3& velocity() const noexcept;

    [[nodiscard]]
    const math::Vec3& acceleration() const noexcept;
    
[[nodiscard]]
const math::Vec3& previousAcceleration()
    const noexcept;

    void setPosition(
        const math::Vec3& position
    ) noexcept;
    void setPreviousAcceleration(
    const math::Vec3& acceleration
) noexcept;

    void setVelocity(
        const math::Vec3& velocity
    ) noexcept;

    void setAcceleration(
        const math::Vec3& acceleration
    ) noexcept;

private:

    std::string name_;

    double mass_;
    double radius_;

    math::Vec3 position_;
    math::Vec3 velocity_;
    math::Vec3 acceleration_;
math::Vec3 previousAcceleration_;
};

} // namespace solarium::celestial
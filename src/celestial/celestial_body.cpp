#include "solarium/celestial/celestial_body.hpp"

#include <utility>

namespace solarium::celestial {

CelestialBody::CelestialBody(
    std::string name,
    double mass,
    double radius,
    const math::Vec3& position,
    const math::Vec3& velocity
)
    : name_(std::move(name)),
      mass_(mass),
      radius_(radius),
      position_(position),
      velocity_(velocity),
      acceleration_{} {
}

const std::string&
CelestialBody::name() const noexcept {
    return name_;
}

double CelestialBody::mass() const noexcept {
    return mass_;
}

double CelestialBody::radius() const noexcept {
    return radius_;
}

const math::Vec3&
CelestialBody::position() const noexcept {
    return position_;
}

const math::Vec3&
CelestialBody::velocity() const noexcept {
    return velocity_;
}

const math::Vec3&
CelestialBody::acceleration() const noexcept {
    return acceleration_;
}

void CelestialBody::setPosition(
    const math::Vec3& position
) noexcept {
    position_ = position;
}

void CelestialBody::setVelocity(
    const math::Vec3& velocity
) noexcept {
    velocity_ = velocity;
}

void CelestialBody::setAcceleration(
    const math::Vec3& acceleration
) noexcept {
    acceleration_ = acceleration;
}

} // namespace solarium::celestial
#include "solarium/celestial/celestial_body.hpp"

#include <cmath>
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
      acceleration_{},
      previousAcceleration_{}
{
}

const std::string&
CelestialBody::name() const noexcept {
    return name_;
}

double
CelestialBody::mass() const noexcept {
    return mass_;
}

double
CelestialBody::radius() const noexcept {
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

const math::Vec3&
CelestialBody::previousAcceleration() const noexcept {
    return previousAcceleration_;
}

BodyType
CelestialBody::type() const noexcept {
    return type_;
}

const std::string&
CelestialBody::parentName() const noexcept {
    return parentName_;
}

const OrbitalParameters&
CelestialBody::orbitalParameters() const noexcept {
    return orbitalParameters_;
}

const RotationParameters&
CelestialBody::rotation() const noexcept {
    return rotation_;
}

const VisualProperties&
CelestialBody::visualProperties() const noexcept {
    return visualProperties_;
}

void
CelestialBody::advanceRotation(
    double elapsedSeconds
) noexcept {
    if (rotation_.period == 0.0) {
        return;
    }

    constexpr double fullTurn = 6.28318530717958647692;
    rotation_.angle += fullTurn * elapsedSeconds / rotation_.period;
    rotation_.angle = std::fmod(rotation_.angle, fullTurn);
}

void
CelestialBody::setMetadata(
    BodyType type,
    std::string parentName,
    const OrbitalParameters& orbitalParameters,
    const RotationParameters& rotation,
    const VisualProperties& visualProperties
) {
    type_ = type;
    parentName_ = std::move(parentName);
    orbitalParameters_ = orbitalParameters;
    rotation_ = rotation;
    visualProperties_ = visualProperties;
}

void
CelestialBody::setPosition(
    const math::Vec3& position
) noexcept {
    position_ = position;
}

void
CelestialBody::setVelocity(
    const math::Vec3& velocity
) noexcept {
    velocity_ = velocity;
}

void
CelestialBody::setAcceleration(
    const math::Vec3& acceleration
) noexcept {
    acceleration_ = acceleration;
}

void
CelestialBody::setPreviousAcceleration(
    const math::Vec3& acceleration
) noexcept {
    previousAcceleration_ = acceleration;
}

} // namespace solarium
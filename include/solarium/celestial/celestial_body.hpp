#pragma once

#include "solarium/math/vec3.hpp"

#include <cstdint>
#include <string>

namespace solarium::celestial {

enum class BodyType : std::uint8_t {
    Star,
    Planet,
    DwarfPlanet,
    Moon,
    Asteroid,
    Comet
};

struct VisualProperties {
    math::Vec3 baseColor{0.7, 0.7, 0.7};
    math::Vec3 atmosphereColor{0.0, 0.0, 0.0};
    double emissive = 0.0;
    double visualRadiusMultiplier = 1.0;
    bool hasAtmosphere = false;
    bool hasRings = false;
};

struct RotationParameters {
    double period = 0.0;
    double axialTilt = 0.0;
    double angle = 0.0;
};

struct OrbitalParameters {
    double semiMajorAxis = 0.0;
    double eccentricity = 0.0;
    double inclination = 0.0;
    double longitudeAscendingNode = 0.0;
    double argumentOfPeriapsis = 0.0;
    double meanAnomaly = 0.0;
    double period = 0.0;
};

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

    [[nodiscard]] BodyType type() const noexcept;
    [[nodiscard]] const std::string& parentName() const noexcept;
    [[nodiscard]] const OrbitalParameters& orbitalParameters() const noexcept;
    [[nodiscard]] const RotationParameters& rotation() const noexcept;
    [[nodiscard]] const VisualProperties& visualProperties() const noexcept;

    void advanceRotation(double elapsedSeconds) noexcept;

    void setMetadata(
        BodyType type,
        std::string parentName,
        const OrbitalParameters& orbitalParameters,
        const RotationParameters& rotation,
        const VisualProperties& visualProperties
    );

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

    BodyType type_ = BodyType::Planet;
    std::string parentName_;
    OrbitalParameters orbitalParameters_;
    RotationParameters rotation_;
    VisualProperties visualProperties_;
};

} // namespace solarium::celestial
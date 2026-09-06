#pragma once

#include "solarium/celestial/celestial_body.hpp"

#include <cstddef>
#include <vector>

namespace solarium::celestial {

struct BodyDefinition {

    const char* name;

    double mass;
    double radius;

    math::Vec3 position;
    math::Vec3 velocity;

    BodyType type = BodyType::Planet;
    const char* parentName = nullptr;
    OrbitalParameters orbitalParameters{};
    RotationParameters rotation{};
    VisualProperties visualProperties{};
};

class BodyRegistry {
public:

    BodyRegistry();

    void initializeSolarSystem();

    void clear();

    void add(
        const BodyDefinition& definition
    );

    [[nodiscard]]
    CelestialBody* find(const std::string& name) noexcept;

    [[nodiscard]]
    const CelestialBody* find(const std::string& name) const noexcept;

    [[nodiscard]]
    std::vector<CelestialBody>&
    bodies() noexcept;

    [[nodiscard]]
    const std::vector<CelestialBody>&
    bodies() const noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept;

private:

    std::vector<CelestialBody> bodies_;
};

} // namespace solarium::celestial
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
#pragma once

#include "solarium/celestial/celestial_body.hpp"
#include "solarium/rendering/camera.hpp"

#include <cstddef>
#include <vector>

namespace solarium::rendering {

class BodyPicker {
public:
    [[nodiscard]]
    static std::size_t pick(
        const std::vector<celestial::CelestialBody>& bodies,
        const Camera& camera,
        double cursorX,
        double cursorY,
        int viewportWidth,
        int viewportHeight
    );
};

} // namespace solarium::rendering

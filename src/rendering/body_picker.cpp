#include "solarium/rendering/body_picker.hpp"

#include "solarium/math/constants.hpp"

#include <cmath>
#include <limits>

namespace solarium::rendering {

namespace {

struct ProjectedPoint {
    double x;
    double y;
    double depth;
    bool visible;
};

ProjectedPoint project(
    const celestial::CelestialBody& body,
    const Camera& camera
) {
    const auto& position = body.position();
    const double scale = math::AstronomicalUnit;
    const float world[4]{
        static_cast<float>(position.x / scale),
        static_cast<float>(position.y / scale),
        static_cast<float>(position.z / scale),
        1.0f
    };
    const float* view = camera.viewMatrix();
    const float* projection = camera.projectionMatrix();
    float viewPosition[4]{};
    float clip[4]{};

    for (int row = 0; row < 4; ++row) {
        viewPosition[row] =
            view[row] * world[0] +
            view[4 + row] * world[1] +
            view[8 + row] * world[2] +
            view[12 + row] * world[3];
    }
    for (int row = 0; row < 4; ++row) {
        clip[row] =
            projection[row] * viewPosition[0] +
            projection[4 + row] * viewPosition[1] +
            projection[8 + row] * viewPosition[2] +
            projection[12 + row] * viewPosition[3];
    }

    if (clip[3] <= 0.0f) {
        return {0.0, 0.0, 0.0, false};
    }

    return {
        (static_cast<double>(clip[0] / clip[3]) + 1.0) * 0.5,
        (1.0 - static_cast<double>(clip[1] / clip[3])) * 0.5,
        static_cast<double>(clip[2] / clip[3]),
        true
    };
}

} // namespace

std::size_t BodyPicker::pick(
    const std::vector<celestial::CelestialBody>& bodies,
    const Camera& camera,
    double cursorX,
    double cursorY,
    int viewportWidth,
    int viewportHeight
) {
    constexpr std::size_t noSelection = std::numeric_limits<std::size_t>::max();
    if (viewportWidth <= 0 || viewportHeight <= 0) {
        return noSelection;
    }

    const double normalizedX = cursorX / static_cast<double>(viewportWidth);
    const double normalizedY = cursorY / static_cast<double>(viewportHeight);
    double bestDistance = 30.0 * 30.0;
    double bestDepth = std::numeric_limits<double>::max();
    std::size_t best = noSelection;

    for (std::size_t index = 0; index < bodies.size(); ++index) {
        const auto projected = project(bodies[index], camera);
        if (!projected.visible || projected.x < -0.05 || projected.x > 1.05 ||
            projected.y < -0.05 || projected.y > 1.05) {
            continue;
        }

        const double dx = (projected.x - normalizedX) * viewportWidth;
        const double dy = (projected.y - normalizedY) * viewportHeight;
        const double distanceSquared = dx * dx + dy * dy;
        if (distanceSquared < bestDistance ||
            (distanceSquared == bestDistance && projected.depth < bestDepth)) {
            bestDistance = distanceSquared;
            bestDepth = projected.depth;
            best = index;
        }
    }

    return best;
}

} // namespace solarium::rendering

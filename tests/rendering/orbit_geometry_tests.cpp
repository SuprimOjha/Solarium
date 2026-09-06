#include "solarium/rendering/orbit_renderer.hpp"
#include "solarium/celestial/body_registry.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    using namespace solarium;

    constexpr double semiMajorAxis = 10.0;
    constexpr double eccentricity = 0.25;
    const celestial::OrbitalParameters orbit{
        semiMajorAxis,
        eccentricity,
        0.35,
        0.7,
        1.1,
        0.0,
        0.0,
        0.0
    };
    const auto points = rendering::OrbitRenderer::generateOrbitPoints(orbit, 256);

    assert(points.size() == 257);
    double minimumRadius = points.front().magnitude();
    double maximumRadius = minimumRadius;
    double minimumZ = points.front().z;
    double maximumZ = points.front().z;
    for (const auto& point : points) {
        minimumRadius = std::min(minimumRadius, point.magnitude());
        maximumRadius = std::max(maximumRadius, point.magnitude());
        minimumZ = std::min(minimumZ, point.z);
        maximumZ = std::max(maximumZ, point.z);
    }

    assert(std::abs(minimumRadius - semiMajorAxis * (1.0 - eccentricity)) < 0.01);
    assert(std::abs(maximumRadius - semiMajorAxis * (1.0 + eccentricity)) < 0.01);
    assert(maximumZ - minimumZ > 1.0);

    celestial::BodyRegistry registry;
    registry.initializeSolarSystem();
    const auto* sun = registry.find("Sun");
    assert(sun != nullptr);
    for (const char* name : {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"}) {
        const auto* body = registry.find(name);
        assert(body != nullptr);
        const auto localPosition = body->position() - sun->position();
        const auto generated = rendering::OrbitRenderer::generateOrbitPoints(
            body->orbitalParameters(),
            512
        );
        double closestDistance = (generated.front() - localPosition).magnitudeSquared();
        for (const auto& point : generated) {
            closestDistance = std::min(
                closestDistance,
                (point - localPosition).magnitudeSquared()
            );
        }
        assert(std::sqrt(closestDistance) < 1.0e6);
    }

    std::cout << "OrbitGeometryTests passed.\n";
    return 0;
}
#include "solarium/rendering/orbit_renderer.hpp"

#include "solarium/math/constants.hpp"

#include <glad/gl.h>

#include <cmath>
#include <algorithm>
#include <vector>

namespace solarium::rendering {

namespace {

struct OrbitVertex {
    float x;
    float y;
    float z;
};

constexpr double Pi = 3.14159265358979323846;

} // namespace

OrbitRenderer::OrbitRenderer(std::size_t segments)
    : segments_(segments < 16 ? 16 : segments),
      buffers_{},
      shader_("assets/shaders/orbit.vert", "assets/shaders/orbit.frag") {
}

OrbitRenderer::~OrbitRenderer() {
    clear();
}

void OrbitRenderer::clear() {
    for (auto& [name, buffer] : buffers_) {
        (void)name;
        if (buffer.vbo != 0) {
            glDeleteBuffers(1, &buffer.vbo);
        }
        if (buffer.vao != 0) {
            glDeleteVertexArrays(1, &buffer.vao);
        }
    }
    buffers_.clear();
}

OrbitRenderer::OrbitBuffer& OrbitRenderer::bufferFor(
    const celestial::CelestialBody& body
) {
    auto [iterator, inserted] = buffers_.try_emplace(body.name());
    if (inserted) {
        buildBuffer(body, iterator->second);
    }
    return iterator->second;
}

void OrbitRenderer::buildBuffer(
    const celestial::CelestialBody& body,
    OrbitBuffer& buffer
) {
    const auto& orbit = body.orbitalParameters();
    if (orbit.semiMajorAxis <= 0.0) {
        return;
    }

    std::vector<OrbitVertex> vertices;
    vertices.reserve(segments_ + 1);

    const auto points = generateOrbitPoints(orbit, segments_);
    const double scale = math::AstronomicalUnit;
    for (const auto& point : points) {
        vertices.push_back({
            static_cast<float>(point.x / scale),
            static_cast<float>(point.y / scale),
            static_cast<float>(point.z / scale)
        });
    }

    buffer.count = vertices.size();
    glGenVertexArrays(1, &buffer.vao);
    glGenBuffers(1, &buffer.vbo);
    glBindVertexArray(buffer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<long long>(vertices.size() * sizeof(OrbitVertex)),
        vertices.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OrbitVertex), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

std::vector<math::Vec3> OrbitRenderer::generateOrbitPoints(
    const celestial::OrbitalParameters& orbit,
    std::size_t segments
) {
    const std::size_t sampleCount = std::max<std::size_t>(segments, 16);
    std::vector<math::Vec3> points;
    points.reserve(sampleCount + 1);

    const double eccentricity = std::clamp(orbit.eccentricity, 0.0, 0.999999);
    const double parameter = orbit.semiMajorAxis *
        (1.0 - eccentricity * eccentricity);
    const double cosNode = std::cos(orbit.longitudeAscendingNode);
    const double sinNode = std::sin(orbit.longitudeAscendingNode);
    const double cosInclination = std::cos(orbit.inclination);
    const double sinInclination = std::sin(orbit.inclination);
    const double cosPeriapsis = std::cos(orbit.argumentOfPeriapsis);
    const double sinPeriapsis = std::sin(orbit.argumentOfPeriapsis);

    for (std::size_t index = 0; index <= sampleCount; ++index) {
        const double trueAnomaly = 2.0 * Pi * static_cast<double>(index) /
            static_cast<double>(sampleCount);
        const double radius = parameter /
            (1.0 + eccentricity * std::cos(trueAnomaly));

        // Perifocal coordinates, followed by Rz(Omega) Rx(i) Rz(omega).
        const double periX = radius * std::cos(trueAnomaly);
        const double periY = radius * std::sin(trueAnomaly);
        const double rotatedPeriX = cosPeriapsis * periX - sinPeriapsis * periY;
        const double rotatedPeriY = sinPeriapsis * periX + cosPeriapsis * periY;
        const double inclinedY = cosInclination * rotatedPeriY;
        const double inclinedZ = sinInclination * rotatedPeriY;

        points.emplace_back(
            cosNode * rotatedPeriX - sinNode * inclinedY,
            sinNode * rotatedPeriX + cosNode * inclinedY,
            inclinedZ
        );
    }

    return points;
}

void OrbitRenderer::render(
    const celestial::CelestialBody& body,
    const Camera& camera,
    const math::Vec3& parentPosition,
    bool visible
) {
    if (!visible || body.orbitalParameters().semiMajorAxis <= 0.0) {
        return;
    }

    auto& buffer = bufferFor(body);
    if (buffer.count == 0) {
        return;
    }

    shader_.bind();
    shader_.setMat4("uView", camera.viewMatrix());
    shader_.setMat4("uProjection", camera.projectionMatrix());
    shader_.setVec3(
        "uOrigin",
        static_cast<float>(parentPosition.x / math::AstronomicalUnit),
        static_cast<float>(parentPosition.y / math::AstronomicalUnit),
        static_cast<float>(parentPosition.z / math::AstronomicalUnit)
    );
    shader_.setVec3("uColor", 0.28f, 0.42f, 0.62f);
    shader_.setFloat("uOpacity", body.type() == celestial::BodyType::Moon ? 0.28f : 0.5f);
    glBindVertexArray(buffer.vao);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(buffer.count));
    glBindVertexArray(0);
    shader_.unbind();
}

} // namespace solarium::rendering

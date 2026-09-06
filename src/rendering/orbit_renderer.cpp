#include "solarium/rendering/orbit_renderer.hpp"

#include "solarium/math/constants.hpp"

#include <glad/gl.h>

#include <cmath>
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

    const double scale = math::AstronomicalUnit;
    const double semiMinorAxis = orbit.semiMajorAxis *
        std::sqrt(std::max(0.0, 1.0 - orbit.eccentricity * orbit.eccentricity));

    for (std::size_t index = 0; index <= segments_; ++index) {
        const double angle = 2.0 * Pi * static_cast<double>(index) /
            static_cast<double>(segments_);
        const double x = orbit.semiMajorAxis * std::cos(angle) / scale;
        const double z = semiMinorAxis * std::sin(angle) / scale;
        vertices.push_back({
            static_cast<float>(x),
            static_cast<float>(z * std::sin(orbit.inclination)),
            static_cast<float>(z * std::cos(orbit.inclination))
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

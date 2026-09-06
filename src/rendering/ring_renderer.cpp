#include "solarium/rendering/ring_renderer.hpp"

#include <glad/gl.h>

#include <cmath>
#include <vector>

namespace solarium::rendering {

namespace {

struct RingVertex {
    float x;
    float y;
    float z;
    float alpha;
};

} // namespace

RingRenderer::RingRenderer()
    : vao_(0), vbo_(0), vertexCount_(0), shader_("assets/shaders/ring.vert", "assets/shaders/ring.frag") {
    createRings();
}

RingRenderer::~RingRenderer() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}

void RingRenderer::createRings() {
    constexpr int segments = 128;
    constexpr int bands = 5;
    constexpr float pi = 3.14159265359f;
    std::vector<RingVertex> vertices;
    vertices.reserve(static_cast<std::size_t>(segments * bands * 6));

    for (int band = 0; band < bands; ++band) {
        const float inner = 0.055f + static_cast<float>(band) * 0.014f;
        const float outer = inner + 0.010f;
        const float alpha = 0.42f - static_cast<float>(band % 2) * 0.14f;
        for (int segment = 0; segment < segments; ++segment) {
            const float a0 = 2.0f * pi * static_cast<float>(segment) / static_cast<float>(segments);
            const float a1 = 2.0f * pi * static_cast<float>(segment + 1) / static_cast<float>(segments);
            const RingVertex p0{inner * std::cos(a0), 0.0f, inner * std::sin(a0), alpha};
            const RingVertex p1{outer * std::cos(a0), 0.0f, outer * std::sin(a0), alpha};
            const RingVertex p2{outer * std::cos(a1), 0.0f, outer * std::sin(a1), alpha};
            const RingVertex p3{inner * std::cos(a1), 0.0f, inner * std::sin(a1), alpha};
            vertices.insert(vertices.end(), {p0, p1, p2, p0, p2, p3});
        }
    }

    vertexCount_ = static_cast<unsigned int>(vertices.size());
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long long>(vertices.size() * sizeof(RingVertex)), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RingVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(RingVertex), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void RingRenderer::render(const celestial::CelestialBody& body, const Camera& camera) {
    if (!body.visualProperties().hasRings) {
        return;
    }

    const auto& position = body.position();
    constexpr float au = 1.495978707e11f;
    shader_.bind();
    shader_.setMat4("uView", camera.viewMatrix());
    shader_.setMat4("uProjection", camera.projectionMatrix());
    shader_.setVec3("uPosition", static_cast<float>(position.x / au), static_cast<float>(position.y / au), static_cast<float>(position.z / au));
    shader_.setFloat("uAxialTilt", static_cast<float>(body.rotation().axialTilt));
    shader_.setVec3("uColor", 0.72f, 0.63f, 0.45f);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertexCount_));
    glBindVertexArray(0);
    shader_.unbind();
}

} // namespace solarium::rendering

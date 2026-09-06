#include "solarium/rendering/star_field_renderer.hpp"

#include <glad/gl.h>

#include <cmath>
#include <random>
#include <vector>

namespace solarium::rendering {

StarFieldRenderer::StarFieldRenderer(std::size_t starCount)
    : vao_(0), vbo_(0), count_(0), shader_("assets/shaders/star.vert", "assets/shaders/star.frag") {
    createStars(starCount);
}

StarFieldRenderer::~StarFieldRenderer() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}

void StarFieldRenderer::createStars(std::size_t count) {
    std::mt19937 generator(0x534f4c41u);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::vector<Star> stars;
    stars.reserve(count);

    for (std::size_t index = 0; index < count; ++index) {
        const float z = 2.0f * unit(generator) - 1.0f;
        const float azimuth = 6.28318530718f * unit(generator);
        const float radial = std::sqrt(1.0f - z * z);
        const float distance = 450.0f + 250.0f * unit(generator);
        stars.push_back({
            radial * std::cos(azimuth) * distance,
            z * distance,
            radial * std::sin(azimuth) * distance,
            0.35f + 0.65f * unit(generator),
            unit(generator)
        });
    }

    count_ = stars.size();
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<long long>(stars.size() * sizeof(Star)),
        stars.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Star), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Star), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Star), reinterpret_cast<void*>(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void StarFieldRenderer::render(const Camera& camera) {
    shader_.bind();
    shader_.setMat4("uView", camera.viewMatrix());
    shader_.setMat4("uProjection", camera.projectionMatrix());
    shader_.setVec3("uCameraPosition", camera.position().x, camera.position().y, camera.position().z);
    glDepthMask(GL_FALSE);
    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, static_cast<int>(count_));
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    shader_.unbind();
}

} // namespace solarium::rendering

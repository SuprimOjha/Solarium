#include "solarium/rendering/orbit_trail.hpp"

#include <glad/gl.h>

#include <cstddef>

namespace solarium::rendering {

OrbitTrail::OrbitTrail(
    std::size_t maxPoints
)
    : points_(),
      maxPoints_(maxPoints),
      vao_(0),
      vbo_(0),
      dirty_(false) {

    initializeOpenGL();
}
OrbitTrail::~OrbitTrail() {

    if (vbo_ != 0) {
        glDeleteBuffers(
            1,
            &vbo_
        );
    }

    if (vao_ != 0) {
        glDeleteVertexArrays(
            1,
            &vao_
        );
    }
}
void OrbitTrail::initializeOpenGL() {

    glGenVertexArrays(
        1,
        &vao_
    );

    glGenBuffers(
        1,
        &vbo_
    );

    glBindVertexArray(vao_);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo_
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<long long>(
            maxPoints_ *
            sizeof(math::Vec3)
        ),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_DOUBLE,
        GL_FALSE,
        sizeof(math::Vec3),
        nullptr
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void OrbitTrail::addPoint(
    const math::Vec3& point
) {

    points_.push_back(point);

    if (points_.size() > maxPoints_) {

        points_.erase(
            points_.begin()
        );
    }

    dirty_ = true;
}

void OrbitTrail::clear() {

    points_.clear();
    dirty_ = true;
}

void OrbitTrail::upload() {

    if (!dirty_) {
        return;
    }

    if (points_.empty()) {
        return;
    }

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo_
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<long long>(
            points_.size() *
            sizeof(math::Vec3)
        ),
        points_.data()
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    dirty_ = false;
}

void OrbitTrail::draw() const {

    if (points_.size() < 2) {
        return;
    }

    glBindVertexArray(vao_);

    glDrawArrays(
        GL_LINE_STRIP,
        0,
        static_cast<int>(
            points_.size()
        )
    );

    glBindVertexArray(0);
}

std::size_t OrbitTrail::size()
    const noexcept {

    return points_.size();
}

}
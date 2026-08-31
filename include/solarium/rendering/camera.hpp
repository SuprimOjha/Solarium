#pragma once

#include "solarium/math/vec3.hpp"

namespace solarium::rendering {

class Camera {
public:
    Camera();

    void update(
        double deltaTime
    );

    void processMouse(
        double xOffset,
        double yOffset
    );

    void processScroll(
        double offset
    );

    [[nodiscard]]
    math::Vec3 position() const noexcept;

    [[nodiscard]]
    const float* viewMatrix() const noexcept;

    [[nodiscard]]
    const float* projectionMatrix() const noexcept;

private:
    float yaw_;
    float pitch_;
    float distance_;

    float view_[16];
    float projection_[16];

    math::Vec3 position_;

    void rebuildView();
    void rebuildProjection();
};

}
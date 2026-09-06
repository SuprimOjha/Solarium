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

    void processMovement(
        double forward,
        double right,
        double up
    );

    void focus(const math::Vec3& target) noexcept;

    void reset() noexcept;

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
    math::Vec3 target_;

    void rebuildView();
    void rebuildProjection();
};

}
#pragma once

#include "solarium/math/vec3.hpp"

namespace solarium::rendering {

enum class CameraMode {
    Free,
    Orbit,
    Follow
};

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

    void processPan(
        double horizontal,
        double vertical
    );

    void setMode(CameraMode mode) noexcept;

    [[nodiscard]] CameraMode mode() const noexcept;

    void setMovementSpeed(float speed) noexcept;
    void setZoomSpeed(float speed) noexcept;

    void setAspectRatio(float aspectRatio) noexcept;

    void frameScene(
        const math::Vec3& center,
        float radius,
        float margin = 1.15f
    ) noexcept;

    [[nodiscard]] float movementSpeed() const noexcept;
    [[nodiscard]] float zoomSpeed() const noexcept;

    void focus(const math::Vec3& target) noexcept;

    void focus(
        const math::Vec3& target,
        float distance
    ) noexcept;

    void setFollowTarget(const math::Vec3& target) noexcept;

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
    float desiredDistance_;
    float movementSpeed_;
    float zoomSpeed_;
    float aspectRatio_;
    CameraMode mode_;

    float view_[16];
    float projection_[16];

    math::Vec3 position_;
    math::Vec3 target_;
    math::Vec3 desiredTarget_;

    void rebuildView();
    void rebuildProjection();
};

}
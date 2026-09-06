#include "solarium/rendering/camera.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float Pi = 3.14159265359f;

float radians(float degrees) {
    return degrees * Pi / 180.0f;
}

void identity(float* matrix) {

    for (int i = 0; i < 16; ++i) {
        matrix[i] = 0.0f;
    }

    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

}

namespace solarium::rendering {

Camera::Camera()
    : yaw_(45.0f),
      pitch_(20.0f),
      distance_(5.0f),
            desiredDistance_(5.0f),
            movementSpeed_(0.08f),
            zoomSpeed_(1.0f),
            mode_(CameraMode::Orbit),
      view_{},
      projection_{},
            position_{},
            target_{},
            desiredTarget_{} {

    identity(view_);
    identity(projection_);

    rebuildView();
    rebuildProjection();
}

void Camera::update(
    double deltaTime
) {
    const float blend = static_cast<float>(
        1.0 - std::exp(-8.0 * std::max(0.0, deltaTime))
    );
    target_ += (desiredTarget_ - target_) * blend;
    distance_ += (desiredDistance_ - distance_) * blend;
    rebuildView();
}

void Camera::processMouse(
    double xOffset,
    double yOffset
) {

    constexpr float sensitivity = 0.15f;

    yaw_ += static_cast<float>(
        xOffset * sensitivity
    );

    pitch_ += static_cast<float>(
        yOffset * sensitivity
    );

    if (pitch_ > 89.0f) {
        pitch_ = 89.0f;
    }

    if (pitch_ < -89.0f) {
        pitch_ = -89.0f;
    }

    rebuildView();
}

void Camera::processScroll(
    double offset
) {

    desiredDistance_ -=
        static_cast<float>(offset) * zoomSpeed_;

    if (desiredDistance_ < 0.02f) {
        desiredDistance_ = 0.02f;
    }

    if (desiredDistance_ > 250.0f) {
        desiredDistance_ = 250.0f;
    }
}

void Camera::processMovement(
    double forward,
    double right,
    double up
) {
    const float yaw = radians(yaw_);
    const math::Vec3 forwardVector{
        -std::cos(yaw),
        0.0,
        -std::sin(yaw)
    };
    const math::Vec3 rightVector{
        -std::sin(yaw),
        0.0,
        std::cos(yaw)
    };

    desiredTarget_ += forwardVector * forward * movementSpeed_;
    desiredTarget_ += rightVector * right * movementSpeed_;
    desiredTarget_.y += up * movementSpeed_;
}

void Camera::processPan(
    double horizontal,
    double vertical
) {
    const float yaw = radians(yaw_);
    const math::Vec3 rightVector{
        -std::sin(yaw),
        0.0,
        std::cos(yaw)
    };
    desiredTarget_ += rightVector * horizontal * movementSpeed_;
    desiredTarget_.y += vertical * movementSpeed_;
}

void Camera::setMode(CameraMode mode) noexcept {
    mode_ = mode;
}

CameraMode Camera::mode() const noexcept {
    return mode_;
}

void Camera::setMovementSpeed(float speed) noexcept {
    movementSpeed_ = std::max(0.001f, speed);
}

void Camera::setZoomSpeed(float speed) noexcept {
    zoomSpeed_ = std::max(0.01f, speed);
}

float Camera::movementSpeed() const noexcept {
    return movementSpeed_;
}

float Camera::zoomSpeed() const noexcept {
    return zoomSpeed_;
}

void Camera::focus(const math::Vec3& target) noexcept {
    desiredTarget_ = target;
}

void Camera::setFollowTarget(const math::Vec3& target) noexcept {
    desiredTarget_ = target;
}

void Camera::reset() noexcept {
    yaw_ = 45.0f;
    pitch_ = 20.0f;
    distance_ = 5.0f;
    desiredDistance_ = 5.0f;
    mode_ = CameraMode::Orbit;
    target_ = {};
    desiredTarget_ = {};
    rebuildView();
}

math::Vec3 Camera::position()
    const noexcept {

    return position_;
}

const float* Camera::viewMatrix()
    const noexcept {

    return view_;
}

const float* Camera::projectionMatrix()
    const noexcept {

    return projection_;
}

void Camera::rebuildView() {

    const float yaw =
        radians(yaw_);

    const float pitch =
        radians(pitch_);

    position_.x =
        distance_ *
        std::cos(pitch) *
        std::cos(yaw);

    position_.y =
        distance_ *
        std::sin(pitch);

    position_.z =
        distance_ *
        std::cos(pitch) *
        std::sin(yaw);

    position_.x += static_cast<float>(target_.x);
    position_.y += static_cast<float>(target_.y);
    position_.z += static_cast<float>(target_.z);

    const float fx =
        static_cast<float>(target_.x) - position_.x;

    const float fy =
        static_cast<float>(target_.y) - position_.y;

    const float fz =
        static_cast<float>(target_.z) - position_.z;

    const float length =
        std::sqrt(
            fx * fx +
            fy * fy +
            fz * fz
        );

    const float nx = fx / length;
    const float ny = fy / length;
    const float nz = fz / length;

    // Right vector.
    const float rx = nz;
    const float ry = 0.0f;
    const float rz = -nx;

    const float rLength =
        std::sqrt(
            rx * rx +
            rz * rz
        );

    const float rnx = rx / rLength;
    const float rny = ry;
    const float rnz = rz / rLength;

    // Up vector.
    const float ux =
        rny * nz - rnz * ny;

    const float uy =
        rnz * nx - rnx * nz;

    const float uz =
        rnx * ny - rny * nx;

    view_[0] = rnx;
    view_[1] = ux;
    view_[2] = -nx;
    view_[3] = 0.0f;

    view_[4] = rny;
    view_[5] = uy;
    view_[6] = -ny;
    view_[7] = 0.0f;

    view_[8] = rnz;
    view_[9] = uz;
    view_[10] = -nz;
    view_[11] = 0.0f;

    view_[12] =
        -(rnx * position_.x +
          rny * position_.y +
          rnz * position_.z);

    view_[13] =
        -(ux * position_.x +
          uy * position_.y +
          uz * position_.z);

    view_[14] =
        nx * position_.x +
        ny * position_.y +
        nz * position_.z;

    view_[15] = 1.0f;
}

void Camera::rebuildProjection() {

    constexpr float fov = 45.0f;
    constexpr float aspect = 16.0f / 9.0f;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 1000.0f;

    const float f =
        1.0f /
        std::tan(
            radians(fov) / 2.0f
        );

    for (float& value : projection_) {
        value = 0.0f;
    }

    projection_[0] =
        f / aspect;

    projection_[5] = f;

    projection_[10] =
        (farPlane + nearPlane) /
        (nearPlane - farPlane);

    projection_[11] = -1.0f;

    projection_[14] =
        (2.0f *
         farPlane *
         nearPlane) /
        (nearPlane - farPlane);
}

}
#include "solarium/rendering/camera.hpp"

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
      view_{},
      projection_{},
      position_{} {

    identity(view_);
    identity(projection_);

    rebuildView();
    rebuildProjection();
}

void Camera::update(
    double
) {
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

    distance_ -=
        static_cast<float>(offset);

    if (distance_ < 1.0f) {
        distance_ = 1.0f;
    }

    if (distance_ > 100.0f) {
        distance_ = 100.0f;
    }

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

    const float fx =
        -position_.x;

    const float fy =
        -position_.y;

    const float fz =
        -position_.z;

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
#include "solarium/math/vec3.hpp"

#include <cmath>

namespace solarium::math {

Vec3& Vec3::operator+=(const Vec3& other) noexcept {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other) noexcept {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3& Vec3::operator*=(double scalar) noexcept {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec3& Vec3::operator/=(double scalar) noexcept {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

double Vec3::magnitudeSquared() const noexcept {
    return
        x * x +
        y * y +
        z * z;
}

double Vec3::magnitude() const noexcept {
    return std::sqrt(
        magnitudeSquared()
    );
}

Vec3 Vec3::normalized() const noexcept {

    const double length = magnitude();

    if (length == 0.0) {
        return Vec3{};
    }

    return *this / length;
}

double Vec3::dot(
    const Vec3& other
) const noexcept {

    return
        x * other.x +
        y * other.y +
        z * other.z;
}

Vec3 Vec3::cross(
    const Vec3& other
) const noexcept {

    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

} // namespace solarium::math
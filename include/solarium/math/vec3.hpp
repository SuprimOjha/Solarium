#pragma once

#include <cmath>

namespace solarium::math {

class Vec3 {
public:
    double x;
    double y;
    double z;

    constexpr Vec3() noexcept
        : x(0.0), y(0.0), z(0.0) {}

    constexpr Vec3(double x_, double y_, double z_) noexcept
        : x(x_), y(y_), z(z_) {}

    constexpr Vec3 operator+(const Vec3& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr Vec3 operator-(const Vec3& other) const noexcept {
        return {x - other.x, y - other.y, z - other.z};
    }

    constexpr Vec3 operator-() const noexcept {
        return {-x, -y, -z};
    }

    constexpr Vec3 operator*(double scalar) const noexcept {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr Vec3 operator/(double scalar) const noexcept {
        return {x / scalar, y / scalar, z / scalar};
    }

    Vec3& operator+=(const Vec3& other) noexcept;
    Vec3& operator-=(const Vec3& other) noexcept;
    Vec3& operator*=(double scalar) noexcept;
    Vec3& operator/=(double scalar) noexcept;

    [[nodiscard]]
    double magnitudeSquared() const noexcept;

    [[nodiscard]]
    double magnitude() const noexcept;

    [[nodiscard]]
    Vec3 normalized() const noexcept;

    [[nodiscard]]
    double dot(const Vec3& other) const noexcept;

    [[nodiscard]]
    Vec3 cross(const Vec3& other) const noexcept;
};

constexpr Vec3 operator*(double scalar, const Vec3& vector) noexcept {
    return vector * scalar;
}

} // namespace solarium::math
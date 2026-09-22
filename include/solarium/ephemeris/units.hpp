#pragma once

#include "solarium/math/vec3.hpp"

namespace solarium::ephemeris {

struct Meters {
    double value;

    explicit constexpr Meters(double value_) noexcept
        : value(value_) {}
};

struct Kilometers {
    double value;

    explicit constexpr Kilometers(double value_) noexcept
        : value(value_) {}
};

struct MetersPerSecond {
    double value;

    explicit constexpr MetersPerSecond(double value_) noexcept
        : value(value_) {}
};

struct KilometersPerSecond {
    double value;

    explicit constexpr KilometersPerSecond(double value_) noexcept
        : value(value_) {}
};

struct Position {
    math::Vec3 meters;

    explicit constexpr Position(math::Vec3 value) noexcept
        : meters(value) {}
};

struct Velocity {
    math::Vec3 metersPerSecond;

    explicit constexpr Velocity(math::Vec3 value) noexcept
        : metersPerSecond(value) {}
};

[[nodiscard]] constexpr Meters toMeters(Kilometers value) noexcept {
    return Meters(value.value * 1'000.0);
}

[[nodiscard]] constexpr Kilometers toKilometers(Meters value) noexcept {
    return Kilometers(value.value / 1'000.0);
}

[[nodiscard]] constexpr MetersPerSecond toMetersPerSecond(
    KilometersPerSecond value
) noexcept {
    return MetersPerSecond(value.value * 1'000.0);
}

[[nodiscard]] constexpr KilometersPerSecond toKilometersPerSecond(
    MetersPerSecond value
) noexcept {
    return KilometersPerSecond(value.value / 1'000.0);
}

} // namespace solarium::ephemeris

#pragma once

namespace solarium::time {

class JulianDate {
public:
    constexpr JulianDate() noexcept
        : value_(0.0) {}

    explicit constexpr JulianDate(double value) noexcept
        : value_(value) {}

    [[nodiscard]]
    constexpr double value() const noexcept {
        return value_;
    }

    constexpr JulianDate& operator+=(double days) noexcept {
        value_ += days;
        return *this;
    }

    [[nodiscard]]
    constexpr JulianDate operator+(double days) const noexcept {
        return JulianDate(value_ + days);
    }

    [[nodiscard]]
    constexpr double operator-(const JulianDate& other) const noexcept {
        return value_ - other.value_;
    }

private:
    double value_;
};

}
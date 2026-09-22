#pragma once

#include <cstdint>

namespace solarium::time {

class JulianDate {
public:
    constexpr JulianDate() noexcept
        : wholeDay_(0), fractionalDay_(0.0) {}

    explicit constexpr JulianDate(double value) noexcept
        : wholeDay_(static_cast<std::int64_t>(value)),
          fractionalDay_(value - static_cast<double>(wholeDay_)) {
        normalize();
    }

    [[nodiscard]]
    static constexpr JulianDate fromParts(
        std::int64_t wholeDay,
        double fractionalDay
    ) noexcept {
        JulianDate result;
        result.wholeDay_ = wholeDay;
        result.fractionalDay_ = fractionalDay;
        result.normalize();
        return result;
    }

    [[nodiscard]]
    constexpr double value() const noexcept {
        return static_cast<double>(wholeDay_) + fractionalDay_;
    }

    [[nodiscard]]
    constexpr std::int64_t wholeDay() const noexcept {
        return wholeDay_;
    }

    [[nodiscard]]
    constexpr double fractionalDay() const noexcept {
        return fractionalDay_;
    }

    constexpr JulianDate& operator+=(double days) noexcept {
        const auto wholeDays = static_cast<std::int64_t>(days);
        wholeDay_ += wholeDays;
        fractionalDay_ += days - static_cast<double>(wholeDays);
        normalize();
        return *this;
    }

    [[nodiscard]]
    constexpr JulianDate operator+(double days) const noexcept {
        JulianDate result = *this;
        result += days;
        return result;
    }

    [[nodiscard]]
    constexpr double operator-(const JulianDate& other) const noexcept {
        return static_cast<double>(wholeDay_ - other.wholeDay_) +
            fractionalDay_ - other.fractionalDay_;
    }

private:
    std::int64_t wholeDay_;
    double fractionalDay_;

    constexpr void normalize() noexcept {
        if (fractionalDay_ >= 1.0 || fractionalDay_ < 0.0) {
            const auto wholeDays = static_cast<std::int64_t>(fractionalDay_);
            wholeDay_ += wholeDays;
            fractionalDay_ -= static_cast<double>(wholeDays);
        }
        if (fractionalDay_ < 0.0) {
            --wholeDay_;
            fractionalDay_ += 1.0;
        }
    }
};

}
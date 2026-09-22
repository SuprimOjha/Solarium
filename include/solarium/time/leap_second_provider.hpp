#pragma once

#include "solarium/time/julian_date.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace solarium::time {

struct LeapSecondEntry {
    JulianDate effectiveUtc;
    std::int32_t taiMinusUtc = 0;
};

class LeapSecondProvider {
public:
    virtual ~LeapSecondProvider() = default;

    [[nodiscard]] virtual std::int32_t taiMinusUtc(JulianDate utc) const = 0;
    [[nodiscard]] virtual JulianDate coverageStart() const noexcept = 0;
    [[nodiscard]] virtual JulianDate coverageEnd() const noexcept = 0;
    [[nodiscard]] virtual std::string_view source() const noexcept = 0;
};

class LeapSecondTable final : public LeapSecondProvider {
public:
    explicit LeapSecondTable(std::vector<LeapSecondEntry> entries = {});

    void replace(std::vector<LeapSecondEntry> entries);
    void append(LeapSecondEntry entry);

    [[nodiscard]] std::int32_t taiMinusUtc(JulianDate utc) const override;
    [[nodiscard]] JulianDate coverageStart() const noexcept override;
    [[nodiscard]] JulianDate coverageEnd() const noexcept override;
    [[nodiscard]] std::string_view source() const noexcept override;

private:
    std::vector<LeapSecondEntry> entries_;
};

class FileLeapSecondProvider final : public LeapSecondProvider {
public:
    explicit FileLeapSecondProvider(const std::string& path);

    [[nodiscard]] std::int32_t taiMinusUtc(JulianDate utc) const override;
    [[nodiscard]] JulianDate coverageStart() const noexcept override;
    [[nodiscard]] JulianDate coverageEnd() const noexcept override;
    [[nodiscard]] std::string_view source() const noexcept override;

private:
    LeapSecondTable table_;
    std::string source_;
};

} // namespace solarium::time

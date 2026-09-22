#include "solarium/time/leap_second_provider.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace solarium::time {
namespace {

void validateEntries(const std::vector<LeapSecondEntry>& entries) {
    if (entries.empty()) {
        throw std::invalid_argument("leap-second table is empty");
    }
    for (std::size_t index = 1; index < entries.size(); ++index) {
        if (entries[index - 1].effectiveUtc.value() >= entries[index].effectiveUtc.value()) {
            throw std::invalid_argument("leap-second entries must be strictly increasing");
        }
    }
}

} // namespace

LeapSecondTable::LeapSecondTable(std::vector<LeapSecondEntry> entries)
    : entries_{} {
    replace(std::move(entries));
}

void LeapSecondTable::replace(std::vector<LeapSecondEntry> entries) {
    std::sort(
        entries.begin(), entries.end(),
        [](const LeapSecondEntry& left, const LeapSecondEntry& right) {
            return left.effectiveUtc.value() < right.effectiveUtc.value();
        }
    );
    if (!entries.empty()) {
        validateEntries(entries);
    }
    entries_ = std::move(entries);
}

void LeapSecondTable::append(LeapSecondEntry entry) {
    auto entries = entries_;
    entries.push_back(entry);
    replace(std::move(entries));
}

std::int32_t LeapSecondTable::taiMinusUtc(JulianDate utc) const {
    if (entries_.empty() || utc.value() < entries_.front().effectiveUtc.value() ||
        utc.value() > entries_.back().effectiveUtc.value() + 1.0e-12) {
        throw std::out_of_range("UTC epoch is outside leap-second provider coverage");
    }

    const auto iterator = std::upper_bound(
        entries_.begin(), entries_.end(), utc.value(),
        [](double value, const LeapSecondEntry& entry) {
            return value < entry.effectiveUtc.value();
        }
    );
    return (iterator == entries_.begin() ? iterator : iterator - 1)->taiMinusUtc;
}

JulianDate LeapSecondTable::coverageStart() const noexcept {
    return entries_.empty() ? JulianDate{} : entries_.front().effectiveUtc;
}

JulianDate LeapSecondTable::coverageEnd() const noexcept {
    return entries_.empty() ? JulianDate{} : entries_.back().effectiveUtc;
}

std::string_view LeapSecondTable::source() const noexcept {
    return "caller-supplied leap-second table";
}

FileLeapSecondProvider::FileLeapSecondProvider(const std::string& path)
    : table_(), source_(path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("unable to open leap-second data source: " + path);
    }

    std::vector<LeapSecondEntry> entries;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line.front() == '#') {
            continue;
        }
        std::istringstream row(line);
        double epoch = 0.0;
        std::int32_t offset = 0;
        if (!(row >> epoch >> offset)) {
            throw std::runtime_error("malformed leap-second data source: " + path);
        }
        entries.push_back({JulianDate(epoch), offset});
    }
    if (entries.empty()) {
        throw std::runtime_error("leap-second data source contains no entries: " + path);
    }
    table_.replace(std::move(entries));
}

std::int32_t FileLeapSecondProvider::taiMinusUtc(JulianDate utc) const {
    return table_.taiMinusUtc(utc);
}

JulianDate FileLeapSecondProvider::coverageStart() const noexcept {
    return table_.coverageStart();
}

JulianDate FileLeapSecondProvider::coverageEnd() const noexcept {
    return table_.coverageEnd();
}

std::string_view FileLeapSecondProvider::source() const noexcept {
    return source_;
}

} // namespace solarium::time

#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace solarium::ephemeris {

enum class EphemerisErrorCode {
    BodyUnavailable,
    EpochOutsideCoverage,
    UnsupportedFrame,
    UnsupportedTimeScale,
    MissingKernel,
    MalformedData,
    ProviderFailure,
    InvalidState
};

class EphemerisError : public std::runtime_error {
public:
    EphemerisError(EphemerisErrorCode code, std::string message)
        : std::runtime_error(std::move(message)), code_(code) {}

    [[nodiscard]] EphemerisErrorCode code() const noexcept {
        return code_;
    }

private:
    EphemerisErrorCode code_;
};

} // namespace solarium::ephemeris

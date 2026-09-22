#pragma once

#include "solarium/ephemeris/canonical_state.hpp"
#include "solarium/ephemeris/ephemeris_provider.hpp"

#include <stdexcept>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace solarium::validation {

enum class ValidationErrorCode {
    BodyMismatch,
    EpochMismatch,
    CenterMismatch,
    FrameMismatch,
    TimeScaleMismatch,
    InvalidStudyRange,
    EmptyStudy
};

class ValidationError : public std::runtime_error {
public:
    ValidationError(ValidationErrorCode code, std::string message);

    [[nodiscard]] ValidationErrorCode code() const noexcept;

private:
    ValidationErrorCode code_;
};

struct ErrorMetrics {
    math::Vec3 positionError{};
    double positionMagnitude = 0.0;
    std::optional<double> relativePositionError;
    math::Vec3 velocityError{};
    double velocityMagnitude = 0.0;
    std::optional<double> relativeVelocityError;
    std::optional<double> radialError;
    std::optional<double> alongTrackError;
    std::optional<double> crossTrackError;
};

struct ValidationSample {
    ephemeris::BodyId body;
    time::JulianDate epoch;
    std::string provider;
    std::string dataset;
    reference::ReferenceFrame frame;
    std::string frameName;
    std::optional<ephemeris::BodyId> center;
    ephemeris::TimeScale timeScale;
    math::Vec3 referencePosition{};
    math::Vec3 solariumPosition{};
    math::Vec3 referenceVelocity{};
    math::Vec3 solariumVelocity{};
    ErrorMetrics metrics;
};

struct ErrorStatistics {
    std::size_t sampleCount = 0;
    double minimumPositionError = 0.0;
    double maximumPositionError = 0.0;
    double meanPositionError = 0.0;
    double rmsPositionError = 0.0;
    double minimumVelocityError = 0.0;
    double maximumVelocityError = 0.0;
    double meanVelocityError = 0.0;
    double rmsVelocityError = 0.0;
};

struct ValidationReport {
    ephemeris::BodyId body;
    std::string provider;
    std::string dataset;
    reference::ReferenceFrame frame;
    std::string frameName;
    std::optional<ephemeris::BodyId> center;
    ephemeris::TimeScale timeScale;
    std::vector<ValidationSample> samples;
    ErrorStatistics statistics;
};

struct ValidationStudy {
    time::JulianDate start;
    time::JulianDate end;
    double stepSeconds = 86'400.0;
    std::optional<ephemeris::BodyId> center;
    reference::ReferenceFrame frame = reference::ReferenceFrame::Barycentric;
    ephemeris::TimeScale timeScale = ephemeris::TimeScale::TDB;
};

using StateSampler = std::function<ephemeris::CanonicalState(time::JulianDate)>;

class ValidationEngine {
public:
    [[nodiscard]] static ValidationSample compare(
        const ephemeris::CanonicalState& solariumState,
        const ephemeris::CanonicalState& referenceState
    );

    [[nodiscard]] static ValidationReport run(
        const ValidationStudy& study,
        ephemeris::BodyId body,
        const ephemeris::EphemerisProvider& provider,
        const StateSampler& solariumSampler
    );

private:
    [[nodiscard]] static ErrorStatistics statistics(
        const std::vector<ValidationSample>& samples
    );
};

} // namespace solarium::validation

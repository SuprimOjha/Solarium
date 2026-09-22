#include "solarium/validation/validation_engine.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace solarium::validation {
namespace {

constexpr double Epsilon = 1.0e-15;
constexpr double SecondsPerDay = 86'400.0;

[[nodiscard]] double magnitude(const math::Vec3& value) noexcept {
    return value.magnitude();
}

[[nodiscard]] std::optional<double> relativeError(
    const math::Vec3& error,
    const math::Vec3& reference
) {
    const double referenceMagnitude = magnitude(reference);
    if (referenceMagnitude <= Epsilon) {
        return std::nullopt;
    }
    return magnitude(error) / referenceMagnitude;
}

void requireCompatible(
    const ephemeris::CanonicalState& solariumState,
    const ephemeris::CanonicalState& referenceState
) {
    if (solariumState.body() != referenceState.body()) {
        throw ValidationError(ValidationErrorCode::BodyMismatch, "validation body mismatch");
    }
    if (solariumState.epoch().value() != referenceState.epoch().value()) {
        throw ValidationError(ValidationErrorCode::EpochMismatch, "validation epoch mismatch");
    }
    if (solariumState.frame() != referenceState.frame()) {
        throw ValidationError(ValidationErrorCode::FrameMismatch, "validation frame mismatch");
    }
    if (solariumState.timeScale() != referenceState.timeScale()) {
        throw ValidationError(ValidationErrorCode::TimeScaleMismatch, "validation time scale mismatch");
    }
    if (solariumState.source().center != referenceState.source().center) {
        throw ValidationError(ValidationErrorCode::CenterMismatch, "validation center mismatch");
    }
}

} // namespace

ValidationError::ValidationError(ValidationErrorCode code, std::string message)
    : std::runtime_error(std::move(message)), code_(code) {}

ValidationErrorCode ValidationError::code() const noexcept {
    return code_;
}

ValidationSample ValidationEngine::compare(
    const ephemeris::CanonicalState& solariumState,
    const ephemeris::CanonicalState& referenceState
) {
    requireCompatible(solariumState, referenceState);

    ValidationSample sample;
    sample.body = referenceState.body();
    sample.epoch = referenceState.epoch();
    sample.provider = referenceState.source().provider;
    sample.dataset = referenceState.source().dataset;
    sample.frame = referenceState.frame();
    sample.frameName = referenceState.source().frameName;
    sample.center = referenceState.source().center;
    sample.timeScale = referenceState.timeScale();
    sample.referencePosition = referenceState.position().meters;
    sample.solariumPosition = solariumState.position().meters;
    sample.referenceVelocity = referenceState.velocity().metersPerSecond;
    sample.solariumVelocity = solariumState.velocity().metersPerSecond;
    sample.metrics.positionError = sample.solariumPosition - sample.referencePosition;
    sample.metrics.positionMagnitude = magnitude(sample.metrics.positionError);
    sample.metrics.relativePositionError = relativeError(
        sample.metrics.positionError,
        sample.referencePosition
    );
    sample.metrics.velocityError = sample.solariumVelocity - sample.referenceVelocity;
    sample.metrics.velocityMagnitude = magnitude(sample.metrics.velocityError);
    sample.metrics.relativeVelocityError = relativeError(
        sample.metrics.velocityError,
        sample.referenceVelocity
    );

    if (sample.referencePosition.magnitude() > Epsilon) {
        const math::Vec3 radial = sample.referencePosition.normalized();
        const double radialComponent = sample.metrics.positionError.dot(radial);
        sample.metrics.radialError = radialComponent;
        const math::Vec3 transverse = sample.referenceVelocity -
            radial * sample.referenceVelocity.dot(radial);
        if (transverse.magnitude() > Epsilon) {
            const math::Vec3 alongTrack = transverse.normalized();
            const math::Vec3 crossTrack = radial.cross(alongTrack).normalized();
            sample.metrics.alongTrackError = sample.metrics.positionError.dot(alongTrack);
            sample.metrics.crossTrackError = sample.metrics.positionError.dot(crossTrack);
        }
    }
    return sample;
}

ValidationReport ValidationEngine::run(
    const ValidationStudy& study,
    ephemeris::BodyId body,
    const ephemeris::EphemerisProvider& provider,
    const StateSampler& solariumSampler
) {
    if (study.stepSeconds <= 0.0 || !std::isfinite(study.stepSeconds) ||
        study.start.value() > study.end.value() || !solariumSampler) {
        throw ValidationError(ValidationErrorCode::InvalidStudyRange, "invalid validation study");
    }

    ValidationReport report;
    report.body = body;
    for (time::JulianDate epoch = study.start;
         epoch.value() <= study.end.value() + 1.0e-12;
         epoch += study.stepSeconds / SecondsPerDay) {
        const ephemeris::CanonicalState referenceState = provider.getState(
            ephemeris::EphemerisRequest{
                body,
                epoch,
                study.center,
                study.frame,
                study.timeScale
            }
        );
        report.samples.push_back(compare(solariumSampler(epoch), referenceState));
    }
    if (report.samples.empty()) {
        throw ValidationError(ValidationErrorCode::EmptyStudy, "validation study produced no samples");
    }
    report.provider = report.samples.front().provider;
    report.dataset = report.samples.front().dataset;
    report.frame = report.samples.front().frame;
    report.frameName = report.samples.front().frameName;
    report.center = report.samples.front().center;
    report.timeScale = report.samples.front().timeScale;
    report.statistics = statistics(report.samples);
    return report;
}

ErrorStatistics ValidationEngine::statistics(
    const std::vector<ValidationSample>& samples
) {
    ErrorStatistics result;
    result.sampleCount = samples.size();
    result.minimumPositionError = samples.front().metrics.positionMagnitude;
    result.minimumVelocityError = samples.front().metrics.velocityMagnitude;
    double positionSum = 0.0;
    double positionSquaredSum = 0.0;
    double velocitySum = 0.0;
    double velocitySquaredSum = 0.0;
    for (const ValidationSample& sample : samples) {
        const double position = sample.metrics.positionMagnitude;
        const double velocity = sample.metrics.velocityMagnitude;
        result.minimumPositionError = std::min(result.minimumPositionError, position);
        result.maximumPositionError = std::max(result.maximumPositionError, position);
        result.minimumVelocityError = std::min(result.minimumVelocityError, velocity);
        result.maximumVelocityError = std::max(result.maximumVelocityError, velocity);
        positionSum += position;
        positionSquaredSum += position * position;
        velocitySum += velocity;
        velocitySquaredSum += velocity * velocity;
    }
    const double count = static_cast<double>(samples.size());
    result.meanPositionError = positionSum / count;
    result.rmsPositionError = std::sqrt(positionSquaredSum / count);
    result.meanVelocityError = velocitySum / count;
    result.rmsVelocityError = std::sqrt(velocitySquaredSum / count);
    return result;
}

} // namespace solarium::validation

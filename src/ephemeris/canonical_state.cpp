#include "solarium/ephemeris/canonical_state.hpp"

#include <cmath>
#include <string>
#include <utility>

namespace solarium::ephemeris {
namespace {

void validateVector(const math::Vec3& value, const char* label) {
    if (!std::isfinite(value.x) || !std::isfinite(value.y) || !std::isfinite(value.z)) {
        throw EphemerisError(
            EphemerisErrorCode::InvalidState,
            std::string(label) + " contains a non-finite component"
        );
    }
}

} // namespace

CanonicalState::CanonicalState(
    BodyId body,
    Position position,
    Velocity velocity,
    time::JulianDate epoch,
    TimeScale timeScale,
    reference::ReferenceFrame frame,
    SourceMetadata source
)
    : body_(body),
      position_(position),
      velocity_(velocity),
      epoch_(epoch),
      timeScale_(timeScale),
      frame_(frame),
      source_(std::move(source)) {
    if (!isKnownBody(body_) || !std::isfinite(epoch_.value())) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "state identity or epoch is invalid");
    }
    validateVector(position_.meters, "position");
    validateVector(velocity_.metersPerSecond, "velocity");
    if (source_.provider.empty()) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "state source provider is empty");
    }
    if (source_.frame != frame_ || source_.timeScale != timeScale_ ||
        source_.epoch.value() != epoch_.value()) {
        throw EphemerisError(
            EphemerisErrorCode::InvalidState,
            "state and source metadata disagree"
        );
    }
}

BodyId CanonicalState::body() const noexcept { return body_; }
const Position& CanonicalState::position() const noexcept { return position_; }
const Velocity& CanonicalState::velocity() const noexcept { return velocity_; }
time::JulianDate CanonicalState::epoch() const noexcept { return epoch_; }
TimeScale CanonicalState::timeScale() const noexcept { return timeScale_; }
reference::ReferenceFrame CanonicalState::frame() const noexcept { return frame_; }
const SourceMetadata& CanonicalState::source() const noexcept { return source_; }

} // namespace solarium::ephemeris

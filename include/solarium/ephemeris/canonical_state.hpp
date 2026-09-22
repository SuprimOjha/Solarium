#pragma once

#include "solarium/ephemeris/body_id.hpp"
#include "solarium/ephemeris/ephemeris_error.hpp"
#include "solarium/ephemeris/units.hpp"
#include "solarium/reference/reference_frame.hpp"
#include "solarium/time/julian_date.hpp"
#include "solarium/time/time_scale.hpp"

#include <optional>
#include <string>

namespace solarium::ephemeris {

using TimeScale = time::TimeScale;

enum class InterpolationStatus {
    Exact,
    Interpolated,
    Unknown
};

struct SourceMetadata {
    std::string provider;
    std::string dataset;
    std::string kernelVersion;
    std::string frameName;
    reference::ReferenceFrame frame = reference::ReferenceFrame::Barycentric;
    std::optional<BodyId> center;
    time::JulianDate epoch{};
    TimeScale timeScale = TimeScale::TDB;
    InterpolationStatus interpolation = InterpolationStatus::Unknown;
};

class CanonicalState {
public:
    CanonicalState(
        BodyId body,
        Position position,
        Velocity velocity,
        time::JulianDate epoch,
        TimeScale timeScale,
        reference::ReferenceFrame frame,
        SourceMetadata source
    );

    [[nodiscard]] BodyId body() const noexcept;
    [[nodiscard]] const Position& position() const noexcept;
    [[nodiscard]] const Velocity& velocity() const noexcept;
    [[nodiscard]] time::JulianDate epoch() const noexcept;
    [[nodiscard]] TimeScale timeScale() const noexcept;
    [[nodiscard]] reference::ReferenceFrame frame() const noexcept;
    [[nodiscard]] const SourceMetadata& source() const noexcept;

private:
    BodyId body_;
    Position position_;
    Velocity velocity_;
    time::JulianDate epoch_;
    TimeScale timeScale_;
    reference::ReferenceFrame frame_;
    SourceMetadata source_;
};

} // namespace solarium::ephemeris

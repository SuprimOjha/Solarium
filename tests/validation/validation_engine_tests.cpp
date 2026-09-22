#include "solarium/validation/validation_engine.hpp"
#include "solarium/validation/validation_export.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string_view>

namespace {

using namespace solarium;

class RecordedProvider final : public ephemeris::EphemerisProvider {
public:
    [[nodiscard]] ephemeris::CanonicalState getState(
        const ephemeris::EphemerisRequest& request
    ) const override {
        ephemeris::SourceMetadata source;
        source.provider = "Recorded Reference";
        source.dataset = "recorded-fixture";
        source.frame = request.frame;
        source.frameName = "J2000";
        source.center = request.center;
        source.epoch = request.epoch;
        source.timeScale = request.timeScale;
        return ephemeris::CanonicalState(
            request.body,
            ephemeris::Position(math::Vec3{request.epoch.value(), 2.0, 3.0}),
            ephemeris::Velocity(math::Vec3{4.0, 5.0, 6.0}),
            request.epoch,
            request.timeScale,
            request.frame,
            source
        );
    }

    [[nodiscard]] ephemeris::CanonicalState getState(
        ephemeris::BodyId body,
        time::JulianDate epoch,
        reference::ReferenceFrame frame
    ) const override {
        return getState({body, epoch, std::nullopt, frame, time::TimeScale::TDB});
    }

    [[nodiscard]] std::string_view providerId() const noexcept override {
        return "Recorded Reference";
    }
};

ephemeris::CanonicalState stateAt(
    time::JulianDate epoch,
    double offset,
    reference::ReferenceFrame frame = reference::ReferenceFrame::Barycentric,
    std::optional<ephemeris::BodyId> center = std::nullopt,
    time::TimeScale scale = time::TimeScale::TDB
) {
    ephemeris::SourceMetadata source;
    source.provider = "Solarium";
    source.dataset = "recorded-fixture";
    source.frame = frame;
    source.frameName = "J2000";
    source.center = center;
    source.epoch = epoch;
    source.timeScale = scale;
    return ephemeris::CanonicalState(
        ephemeris::BodyId::Earth,
        ephemeris::Position(math::Vec3{epoch.value() + offset, 2.0, 3.0}),
        ephemeris::Velocity(math::Vec3{4.0 + offset, 5.0, 6.0}),
        epoch, scale, frame, source
    );
}

} // namespace

int main() {
    using namespace solarium;
    const auto epoch = time::JulianDate(2'451'545.0);
    const auto reference = stateAt(epoch, 0.0);
    const auto solariumState = stateAt(epoch, 10.0, reference::ReferenceFrame::Barycentric);
    const auto sample = validation::ValidationEngine::compare(solariumState, reference);
    assert(sample.metrics.positionMagnitude == 10.0);
    assert(sample.metrics.velocityMagnitude == 10.0);
    assert(sample.metrics.relativePositionError.has_value());
    assert(sample.metrics.radialError.has_value());

    RecordedProvider provider;
    const validation::ValidationReport report = validation::ValidationEngine::run(
        {epoch, epoch + 2.0 / 86'400.0, 1.0},
        ephemeris::BodyId::Earth,
        provider,
        [](time::JulianDate current) { return stateAt(current, 0.5); }
    );
    assert(report.samples.size() == 3);
    assert(report.statistics.sampleCount == 3);
    assert(report.statistics.minimumPositionError == 0.5);
    assert(report.statistics.maximumPositionError == 0.5);
    assert(!validation::toCsv(report).empty());
    assert(validation::toJson(report).find("rmsPositionError") != std::string::npos);

    bool rejected = false;
    try {
        validation::ValidationEngine::compare(
            stateAt(epoch, 0.0, reference::ReferenceFrame::Heliocentric),
            reference
        );
    } catch (const validation::ValidationError& error) {
        rejected = error.code() == validation::ValidationErrorCode::FrameMismatch;
    }
    assert(rejected);

    std::cout << "ValidationEngineTests passed.\n";
    return 0;
}

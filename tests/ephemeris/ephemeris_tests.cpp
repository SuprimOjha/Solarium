#include "solarium/ephemeris/ephemeris_provider.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string_view>

namespace {

using namespace solarium;
using ephemeris::BodyId;
using ephemeris::CanonicalState;
using ephemeris::EphemerisError;
using ephemeris::EphemerisErrorCode;
using ephemeris::InterpolationStatus;
using ephemeris::Position;
using ephemeris::SourceMetadata;
using ephemeris::TimeScale;
using ephemeris::Velocity;
using reference::ReferenceFrame;
using time::JulianDate;

SourceMetadata sourceFor(JulianDate epoch, ReferenceFrame frame) {
    SourceMetadata source;
    source.provider = "test-provider";
    source.dataset = "test-dataset";
    source.kernelVersion = "test-version";
    source.frame = frame;
    source.center = BodyId::Sun;
    source.epoch = epoch;
    source.timeScale = TimeScale::TDB;
    source.interpolation = InterpolationStatus::Interpolated;
    return source;
}

class TestProvider final : public ephemeris::EphemerisProvider {
public:
    CanonicalState getState(
        BodyId body,
        JulianDate epoch,
        ReferenceFrame frame
    ) const override {
        if (body != BodyId::Earth) {
            throw EphemerisError(
                EphemerisErrorCode::BodyUnavailable,
                "test body unavailable"
            );
        }
        return CanonicalState(
            body,
            Position(math::Vec3{1.0, 2.0, 3.0}),
            Velocity(math::Vec3{4.0, 5.0, 6.0}),
            epoch,
            TimeScale::TDB,
            frame,
            sourceFor(epoch, frame)
        );
    }

    [[nodiscard]] std::string_view providerId() const noexcept override {
        return "test-provider";
    }
};

} // namespace

int main() {
    assert(ephemeris::isKnownBody(BodyId::Sun));
    assert(ephemeris::isKnownBody(BodyId::Triton));
    assert(ephemeris::bodyName(BodyId::Ganymede) == "Ganymede");
    assert(!ephemeris::isKnownBody(static_cast<BodyId>(999)));

    const auto kilometers = ephemeris::toKilometers(ephemeris::Meters(12'000.0));
    assert(kilometers.value == 12.0);
    assert(ephemeris::toMeters(kilometers).value == 12'000.0);

    TestProvider provider;
    const JulianDate epoch(2'451'545.0);
    const auto state = provider.getState(
        BodyId::Earth,
        epoch,
        ReferenceFrame::Heliocentric
    );
    assert(provider.providerId() == "test-provider");
    assert(state.body() == BodyId::Earth);
    assert(state.position().meters.x == 1.0);
    assert(state.velocity().metersPerSecond.z == 6.0);
    assert(state.epoch().value() == epoch.value());
    assert(state.timeScale() == TimeScale::TDB);
    assert(state.frame() == ReferenceFrame::Heliocentric);
    assert(state.source().provider == "test-provider");
    assert(state.source().center == BodyId::Sun);
    assert(state.source().interpolation == InterpolationStatus::Interpolated);

    bool unavailable = false;
    try {
        provider.getState(BodyId::Moon, epoch, ReferenceFrame::Heliocentric);
    } catch (const EphemerisError& error) {
        unavailable = error.code() == EphemerisErrorCode::BodyUnavailable;
    }
    assert(unavailable);

    bool invalid = false;
    try {
        SourceMetadata source = sourceFor(epoch, ReferenceFrame::Barycentric);
        CanonicalState badState(
            BodyId::Earth,
            Position(math::Vec3{NAN, 0.0, 0.0}),
            Velocity(math::Vec3{}),
            epoch,
            TimeScale::TDB,
            ReferenceFrame::Barycentric,
            source
        );
        (void)badState;
    } catch (const EphemerisError& error) {
        invalid = error.code() == EphemerisErrorCode::InvalidState;
    }
    assert(invalid);

    std::cout << "EphemerisTests passed.\n";
    return 0;
}

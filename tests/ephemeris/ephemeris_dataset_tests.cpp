#include "solarium/ephemeris/ephemeris_dataset.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

namespace {

solarium::ephemeris::CanonicalState sample(
    solarium::time::JulianDate epoch,
    double position,
    double velocity
) {
    using namespace solarium;
    ephemeris::SourceMetadata source;
    source.provider = "mock";
    source.dataset = "cubic";
    source.frame = reference::ReferenceFrame::Barycentric;
    source.frameName = "J2000";
    source.epoch = epoch;
    source.timeScale = time::TimeScale::TDB;
    return ephemeris::CanonicalState(
        ephemeris::BodyId::Earth,
        ephemeris::Position(math::Vec3{position, 0.0, 0.0}),
        ephemeris::Velocity(math::Vec3{velocity, 0.0, 0.0}),
        epoch,
        time::TimeScale::TDB,
        reference::ReferenceFrame::Barycentric,
        source
    );
}

} // namespace

int main() {
    using namespace solarium;
    const time::JulianDate start(2'451'545.0);
    const time::JulianDate finish = start + 10.0 / 86'400.0;
    ephemeris::EphemerisDatasetKey key{
        "mock", "cubic", ephemeris::BodyId::Earth, std::nullopt,
        reference::ReferenceFrame::Barycentric, time::TimeScale::TDB,
        start, finish, 10.0
    };
    auto dataset = std::make_shared<ephemeris::EphemerisDataset>(key);
    dataset->addSample(sample(start, 0.0, 0.0));
    dataset->addSample(sample(finish, 1'000.0, 300.0));

    const auto exact = dataset->stateAt(ephemeris::BodyId::Earth, finish);
    assert(exact.position().meters.x == 1'000.0);

    const auto midpoint = dataset->stateAt(
        ephemeris::BodyId::Earth,
        start + 5.0 / 86'400.0
    );
    assert(std::abs(midpoint.position().meters.x - 125.0) < 1.0e-9);
    assert(std::abs(midpoint.velocity().metersPerSecond.x - 75.0) < 1.0e-9);
    assert(midpoint.source().interpolation == ephemeris::InterpolationStatus::Interpolated);

    ephemeris::InMemoryEphemerisDatasetCache cache(1);
    cache.store(dataset);
    assert(cache.find(key) != nullptr);

    bool rejected = false;
    try {
        dataset->stateAt(ephemeris::BodyId::Earth, start - 1.0 / 86'400.0);
    } catch (const ephemeris::EphemerisError& error) {
        rejected = error.code() == ephemeris::EphemerisErrorCode::EpochOutsideCoverage;
    }
    assert(rejected);

    std::cout << "EphemerisDatasetTests passed.\n";
    return 0;
}

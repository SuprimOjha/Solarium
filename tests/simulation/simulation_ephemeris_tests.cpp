#include "solarium/simulation/simulation.hpp"

#include <cassert>
#include <iostream>
#include <string>

namespace {

class MockProvider final : public solarium::ephemeris::EphemerisProvider {
public:
    [[nodiscard]] solarium::ephemeris::CanonicalState getState(
        const solarium::ephemeris::EphemerisRequest& request
    ) const override {
        ++queries;
        solarium::ephemeris::SourceMetadata source;
        source.provider = "Mock Ephemeris";
        source.dataset = "deterministic simulation fixture";
        source.frame = request.frame;
        source.frameName = "MOCK-INERTIAL";
        source.center = request.center;
        source.epoch = request.epoch;
        source.timeScale = request.timeScale;
        source.interpolation = solarium::ephemeris::InterpolationStatus::Exact;
        const double value = static_cast<double>(
            static_cast<int>(request.body) + 1
        ) * 1'000.0;
        return solarium::ephemeris::CanonicalState(
            request.body,
            solarium::ephemeris::Position(solarium::math::Vec3{value, 0.0, 0.0}),
            solarium::ephemeris::Velocity(solarium::math::Vec3{}),
            request.epoch,
            request.timeScale,
            request.frame,
            source
        );
    }

    [[nodiscard]] solarium::ephemeris::CanonicalState getState(
        solarium::ephemeris::BodyId body,
        solarium::time::JulianDate epoch,
        solarium::reference::ReferenceFrame frame
    ) const override {
        return getState({
            body, epoch, std::nullopt, frame, solarium::time::TimeScale::TDB
        });
    }

    [[nodiscard]] std::string_view providerId() const noexcept override {
        return "Mock Ephemeris";
    }

    mutable int queries = 0;
};

} // namespace

int main() {
    using namespace solarium;
    auto provider = std::make_shared<MockProvider>();
    simulation::SimulationConfig config;
    config.mode = simulation::SimulationMode::Hybrid;
    config.ephemerisProvider = provider;
    config.ephemerisFrame = reference::ReferenceFrame::Barycentric;
    config.ephemerisTimeScale = time::TimeScale::TDB;

    simulation::Simulation simulation(config);
    assert(provider->queries == 29);
    assert(simulation.mode() == simulation::SimulationMode::Hybrid);
    assert(simulation.ephemerisProviderId() == "Mock Ephemeris");
    assert(simulation.ephemerisError().empty());

    const auto* earth = simulation.ephemerisState(ephemeris::BodyId::Earth);
    assert(earth != nullptr);
    assert(earth->position().meters.x == 4'000.0);
    assert(earth->frame() == reference::ReferenceFrame::Barycentric);
    assert(earth->source().provider == "Mock Ephemeris");
    assert(earth->source().frameName == "MOCK-INERTIAL");

    const double initialEpoch = simulation.currentEpoch().julianDay();
    simulation.update(0.001);
    assert(simulation.currentEpoch().julianDay() > initialEpoch);
    assert(provider->queries == 29);

    std::cout << "SimulationEphemerisTests passed.\n";
    return 0;
}

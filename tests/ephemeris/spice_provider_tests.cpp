#include "solarium/ephemeris/spice_provider.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace solarium;
using ephemeris::BodyId;
using ephemeris::EphemerisError;
using ephemeris::EphemerisErrorCode;
using ephemeris::EphemerisRequest;
using ephemeris::KernelManager;
using ephemeris::SpiceCoverage;
using ephemeris::SpiceKernelCategory;
using ephemeris::SpiceKernelProvenance;
using ephemeris::SpiceKernelSpec;
using ephemeris::SpiceState;
using ephemeris::SpiceStateBackend;
using ephemeris::TimeScale;
using reference::ReferenceFrame;
using time::JulianDate;

class MockKernelManager final : public KernelManager {
public:
    void loadConfiguredKernels() override { loaded = true; }
    void unloadConfiguredKernels() override { loaded = false; }

    void load(const SpiceKernelSpec&) override { loaded = true; }
    void unload(const SpiceKernelSpec&) override { loaded = false; }
    [[nodiscard]] bool isLoaded(const SpiceKernelSpec&) const override { return loaded; }

    [[nodiscard]] bool hasCoverage(int, double) const override {
        return loaded && coverageEnabled;
    }

    [[nodiscard]] std::optional<SpiceCoverage> coverage(int) const override {
        if (!loaded || !coverageEnabled) {
            return std::nullopt;
        }
        return SpiceCoverage{-1.0e9, 1.0e9};
    }

    [[nodiscard]] std::vector<SpiceKernelProvenance> provenance() const override {
        if (!loaded) {
            return {};
        }
        return {{"kernels/test.bsp", SpiceKernelCategory::SPK}};
    }

    bool loaded = true;
    bool coverageEnabled = true;
};

class MockSpiceBackend final : public SpiceStateBackend {
public:
    [[nodiscard]] double toEphemerisTime(
        JulianDate epoch,
        TimeScale timeScale
    ) const override {
        lastTimeScale = timeScale;
        return epoch.value() + (timeScale == TimeScale::UTC ? 1.0 : 2.0);
    }

    [[nodiscard]] SpiceState queryState(
        int targetNaifId,
        int centerNaifId,
        double ephemerisTime,
        std::string_view frameName
    ) const override {
        lastTarget = targetNaifId;
        lastCenter = centerNaifId;
        return SpiceState{
            {1.0, 2.0, 3.0},
            {4.0, 5.0, 6.0},
            std::string(frameName),
            centerNaifId,
            ephemerisTime
        };
    }

    mutable TimeScale lastTimeScale = TimeScale::TDB;
    mutable int lastTarget = 0;
    mutable int lastCenter = 0;
};

} // namespace

int main() {
    assert(ephemeris::spiceBodyId(BodyId::Earth) == 399);
    assert(ephemeris::spiceBodyId(BodyId::Triton) == 801);
    assert(ephemeris::spiceBodyId(static_cast<BodyId>(999)) == 0);

    auto kernels = std::make_shared<MockKernelManager>();
    auto backend = std::make_shared<MockSpiceBackend>();
    ephemeris::SpiceConfiguration configuration;
    configuration.frameName = "J2000";
    ephemeris::SpiceEphemerisProvider provider(configuration, kernels, backend);

    provider.loadConfiguredKernels();
    const auto state = provider.getState(EphemerisRequest{
        BodyId::Earth,
        JulianDate(2'451'545.0),
        BodyId::Sun,
        ReferenceFrame::Heliocentric,
        TimeScale::UTC
    });

    assert(provider.providerId() == "NAIF SPICE");
    assert(backend->lastTimeScale == TimeScale::UTC);
    assert(backend->lastTarget == 399);
    assert(backend->lastCenter == 10);
    assert(state.position().meters.x == 1'000.0);
    assert(state.velocity().metersPerSecond.z == 6'000.0);
    assert(state.source().provider == "NAIF SPICE");
    assert(state.source().kernelVersion == "SPK:kernels/test.bsp");
    assert(state.source().center == BodyId::Sun);

    kernels->coverageEnabled = false;
    bool outsideCoverage = false;
    try {
        (void)provider.getState(EphemerisRequest{
            BodyId::Earth,
            JulianDate(2'451'546.0),
            BodyId::Sun,
            ReferenceFrame::Heliocentric,
            TimeScale::TDB
        });
    } catch (const EphemerisError& error) {
        outsideCoverage = error.code() == EphemerisErrorCode::EpochOutsideCoverage;
    }
    assert(outsideCoverage);

    kernels->loaded = false;
    bool missingKernel = false;
    try {
        (void)provider.getState(BodyId::Earth, JulianDate(2'451'545.0));
    } catch (const EphemerisError& error) {
        missingKernel = error.code() == EphemerisErrorCode::MissingKernel;
    }
    assert(missingKernel);

    std::cout << "SpiceProviderTests passed.\n";
    return 0;
}

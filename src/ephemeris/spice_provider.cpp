#include "solarium/ephemeris/spice_provider.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <cmath>
#include <sstream>
#include <utility>

namespace solarium::ephemeris {
namespace {

struct BodyMapping {
    BodyId body;
    int naifId;
};

constexpr BodyMapping bodyMappings[] = {
    {BodyId::Sun, 10}, {BodyId::Mercury, 199}, {BodyId::Venus, 299},
    {BodyId::Earth, 399}, {BodyId::Moon, 301}, {BodyId::Mars, 499},
    {BodyId::Phobos, 401}, {BodyId::Deimos, 402}, {BodyId::Jupiter, 599},
    {BodyId::Io, 501}, {BodyId::Europa, 502}, {BodyId::Ganymede, 503},
    {BodyId::Callisto, 504}, {BodyId::Saturn, 699}, {BodyId::Mimas, 601},
    {BodyId::Enceladus, 602}, {BodyId::Tethys, 603}, {BodyId::Dione, 604},
    {BodyId::Rhea, 605}, {BodyId::Titan, 606}, {BodyId::Iapetus, 608},
    {BodyId::Uranus, 799}, {BodyId::Miranda, 701}, {BodyId::Ariel, 702},
    {BodyId::Umbriel, 703}, {BodyId::Titania, 704}, {BodyId::Oberon, 705},
    {BodyId::Neptune, 899}, {BodyId::Triton, 801}
};

[[nodiscard]] int centerNaifId(
    std::optional<BodyId> center,
    reference::ReferenceFrame frame
) {
    if (center.has_value()) {
        const int id = spiceBodyId(*center);
        if (id == 0) {
            throw EphemerisError(EphemerisErrorCode::BodyUnavailable, "center has no NAIF mapping");
        }
        return id;
    }
    switch (frame) {
    case reference::ReferenceFrame::Barycentric: return 0;
    case reference::ReferenceFrame::Heliocentric: return 10;
    case reference::ReferenceFrame::Geocentric: return 399;
    case reference::ReferenceFrame::PlanetCentered:
    case reference::ReferenceFrame::MoonCentered:
        throw EphemerisError(
            EphemerisErrorCode::UnsupportedFrame,
            "centered SPICE frames require an explicit center"
        );
    }
    throw EphemerisError(EphemerisErrorCode::UnsupportedFrame, "unsupported SPICE reference frame");
}

[[nodiscard]] const char* categoryName(SpiceKernelCategory category) noexcept {
    switch (category) {
    case SpiceKernelCategory::SPK: return "SPK";
    case SpiceKernelCategory::PCK: return "PCK";
    case SpiceKernelCategory::LSK: return "LSK";
    case SpiceKernelCategory::FK: return "FK";
    case SpiceKernelCategory::MetaKernel: return "META";
    }
    return "UNKNOWN";
}

} // namespace

int spiceBodyId(BodyId body) noexcept {
    for (const BodyMapping& mapping : bodyMappings) {
        if (mapping.body == body) {
            return mapping.naifId;
        }
    }
    return 0;
}

std::string spiceProvenance(const std::vector<SpiceKernelProvenance>& kernels) {
    std::ostringstream result;
    for (std::size_t index = 0; index < kernels.size(); ++index) {
        if (index != 0) {
            result << ';';
        }
        result << categoryName(kernels[index].category) << ':' << kernels[index].path;
    }
    return result.str();
}

SpiceEphemerisProvider::SpiceEphemerisProvider(
    SpiceConfiguration configuration,
    std::shared_ptr<KernelManager> kernelManager,
    std::shared_ptr<SpiceStateBackend> backend
)
    : configuration_(std::move(configuration)),
      kernelManager_(std::move(kernelManager)),
      backend_(std::move(backend)) {
    if (configuration_.frameName.empty() || !kernelManager_ || !backend_) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid SPICE provider configuration");
    }
}

void SpiceEphemerisProvider::loadConfiguredKernels() {
    kernelManager_->loadConfiguredKernels();
}

void SpiceEphemerisProvider::unloadConfiguredKernels() {
    kernelManager_->unloadConfiguredKernels();
}

CanonicalState SpiceEphemerisProvider::getState(
    const EphemerisRequest& request
) const {
    const int target = spiceBodyId(request.body);
    if (target == 0) {
        throw EphemerisError(EphemerisErrorCode::BodyUnavailable, "body has no NAIF mapping");
    }
    if (request.timeScale != TimeScale::UTC && request.timeScale != TimeScale::TT &&
        request.timeScale != TimeScale::TDB) {
        throw EphemerisError(EphemerisErrorCode::UnsupportedTimeScale, "unsupported SPICE time scale");
    }
    if (!std::isfinite(request.epoch.value())) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid SPICE request epoch");
    }

    const int center = centerNaifId(request.center, request.frame);
    const double ephemerisTime = backend_->toEphemerisTime(request.epoch, request.timeScale);
    const auto provenance = kernelManager_->provenance();
    if (provenance.empty()) {
        throw EphemerisError(EphemerisErrorCode::MissingKernel, "no SPICE kernels are loaded");
    }
    if (!kernelManager_->hasCoverage(target, ephemerisTime) ||
        (center != 0 && !kernelManager_->hasCoverage(center, ephemerisTime))) {
        throw EphemerisError(EphemerisErrorCode::EpochOutsideCoverage, "SPICE kernels do not cover the requested epoch");
    }

    const SpiceState state = backend_->queryState(
        target, center, ephemerisTime, configuration_.frameName
    );
    if (state.frame != configuration_.frameName || state.centerNaifId != center ||
        !std::isfinite(state.ephemerisTime) ||
        std::abs(state.ephemerisTime - ephemerisTime) > 1.0e-6) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "SPICE returned inconsistent state metadata");
    }

    SourceMetadata source;
    source.provider = "NAIF SPICE";
    source.dataset = "SPK state vector";
    source.kernelVersion = spiceProvenance(provenance);
    source.frameName = state.frame;
    source.frame = request.frame;
    source.center = request.center;
    source.epoch = request.epoch;
    source.timeScale = request.timeScale;
    source.interpolation = InterpolationStatus::Exact;
    return CanonicalState(
        request.body,
        Position(math::Vec3{
            state.positionKilometers[0] * 1'000.0,
            state.positionKilometers[1] * 1'000.0,
            state.positionKilometers[2] * 1'000.0
        }),
        Velocity(math::Vec3{
            state.velocityKilometersPerSecond[0] * 1'000.0,
            state.velocityKilometersPerSecond[1] * 1'000.0,
            state.velocityKilometersPerSecond[2] * 1'000.0
        }),
        request.epoch,
        request.timeScale,
        request.frame,
        std::move(source)
    );
}

CanonicalState SpiceEphemerisProvider::getState(
    BodyId body,
    time::JulianDate epoch,
    reference::ReferenceFrame frame
) const {
    return getState(EphemerisRequest{
        body, epoch, configuration_.defaultCenter, frame, configuration_.defaultTimeScale
    });
}

CanonicalState SpiceEphemerisProvider::getState(
    BodyId body,
    time::JulianDate epoch
) const {
    return getState(EphemerisRequest{
        body, epoch, configuration_.defaultCenter, configuration_.defaultFrame,
        configuration_.defaultTimeScale
    });
}

std::string_view SpiceEphemerisProvider::providerId() const noexcept {
    return "NAIF SPICE";
}

const SpiceConfiguration& SpiceEphemerisProvider::configuration() const noexcept {
    return configuration_;
}

} // namespace solarium::ephemeris

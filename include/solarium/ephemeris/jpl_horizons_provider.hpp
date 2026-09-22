#pragma once

#include "solarium/ephemeris/ephemeris_provider.hpp"
#include "solarium/ephemeris/http_client.hpp"
#include "solarium/ephemeris/state_cache.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace solarium::ephemeris {

enum class HorizonsOutputUnits {
    KilometersAndSeconds
};

struct JplHorizonsConfiguration {
    std::string apiUrl = "https://ssd.jpl.nasa.gov/api/horizons.api";
    std::chrono::milliseconds timeout{10'000};
    std::string userAgent = "Solarium/2.0";
    std::optional<BodyId> defaultCenter = BodyId::Sun;
    reference::ReferenceFrame defaultFrame = reference::ReferenceFrame::Heliocentric;
    TimeScale defaultTimeScale = TimeScale::TDB;
    HorizonsOutputUnits outputUnits = HorizonsOutputUnits::KilometersAndSeconds;
};

class JplHorizonsProvider final : public EphemerisProvider {
public:
    explicit JplHorizonsProvider(
        const JplHorizonsConfiguration& configuration = {},
        std::shared_ptr<HttpClient> httpClient = std::make_shared<CurlHttpClient>(),
        std::shared_ptr<StateCache> cache = std::make_shared<InMemoryStateCache>()
    );

    [[nodiscard]] CanonicalState getState(
        const EphemerisRequest& request
    ) const override;

    [[nodiscard]] CanonicalState getState(
        BodyId body,
        time::JulianDate epoch
    ) const;

    [[nodiscard]] CanonicalState getState(
        BodyId body,
        time::JulianDate epoch,
        reference::ReferenceFrame frame
    ) const override;

    [[nodiscard]] std::string_view providerId() const noexcept override;

    [[nodiscard]] const JplHorizonsConfiguration& configuration() const noexcept;

private:
    JplHorizonsConfiguration configuration_;
    std::shared_ptr<HttpClient> httpClient_;
    std::shared_ptr<StateCache> cache_;
};

[[nodiscard]] std::string_view horizonsBodyId(BodyId body) noexcept;

} // namespace solarium::ephemeris

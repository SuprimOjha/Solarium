#pragma once

#include "solarium/ephemeris/ephemeris_provider.hpp"
#include "solarium/ephemeris/spice_backend.hpp"
#include "solarium/ephemeris/spice_kernel_manager.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace solarium::ephemeris {

struct SpiceConfiguration {
    SpiceKernelConfiguration kernels;
    std::optional<BodyId> defaultCenter = BodyId::Sun;
    reference::ReferenceFrame defaultFrame = reference::ReferenceFrame::Heliocentric;
    TimeScale defaultTimeScale = TimeScale::TDB;
    std::string frameName = "J2000";
};

class SpiceEphemerisProvider final : public EphemerisProvider {
public:
    explicit SpiceEphemerisProvider(
        SpiceConfiguration configuration = {},
        std::shared_ptr<KernelManager> kernelManager = std::make_shared<SpiceKernelManager>(),
        std::shared_ptr<SpiceStateBackend> backend = std::make_shared<CspiceStateBackend>()
    );

    void loadConfiguredKernels();
    void unloadConfiguredKernels();

    [[nodiscard]] CanonicalState getState(
        const EphemerisRequest& request
    ) const override;

    [[nodiscard]] CanonicalState getState(
        BodyId body,
        time::JulianDate epoch,
        reference::ReferenceFrame frame
    ) const override;

    [[nodiscard]] CanonicalState getState(
        BodyId body,
        time::JulianDate epoch
    ) const;

    [[nodiscard]] std::string_view providerId() const noexcept override;
    [[nodiscard]] const SpiceConfiguration& configuration() const noexcept;

private:
    SpiceConfiguration configuration_;
    std::shared_ptr<KernelManager> kernelManager_;
    std::shared_ptr<SpiceStateBackend> backend_;
};

[[nodiscard]] int spiceBodyId(BodyId body) noexcept;
[[nodiscard]] std::string spiceProvenance(
    const std::vector<SpiceKernelProvenance>& kernels
);

} // namespace solarium::ephemeris

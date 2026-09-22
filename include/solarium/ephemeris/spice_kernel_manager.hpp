#pragma once

#include "solarium/ephemeris/body_id.hpp"
#include "solarium/time/julian_date.hpp"

#include <optional>
#include <string>
#include <vector>

namespace solarium::ephemeris {

enum class SpiceKernelCategory {
    SPK,
    PCK,
    LSK,
    FK
};

struct SpiceKernelSpec {
    std::string relativePath;
    SpiceKernelCategory category;
};

struct SpiceKernelConfiguration {
    std::string kernelDirectory = "kernels";
    std::string metaKernel;
    std::vector<SpiceKernelSpec> loadedKernels;
};

struct SpiceKernelProvenance {
    std::string path;
    SpiceKernelCategory category;
};

struct SpiceCoverage {
    double startEphemerisTime = 0.0;
    double endEphemerisTime = 0.0;
};

class KernelManager {
public:
    virtual ~KernelManager() = default;

    virtual void load(const SpiceKernelSpec& kernel) = 0;
    virtual void unload(const SpiceKernelSpec& kernel) = 0;
    [[nodiscard]] virtual bool isLoaded(const SpiceKernelSpec& kernel) const = 0;
    [[nodiscard]] virtual bool hasCoverage(int naifId, double ephemerisTime) const = 0;
    [[nodiscard]] virtual std::optional<SpiceCoverage> coverage(int naifId) const = 0;
    [[nodiscard]] virtual std::vector<SpiceKernelProvenance> provenance() const = 0;
};

class SpiceKernelManager final : public KernelManager {
public:
    explicit SpiceKernelManager(SpiceKernelConfiguration configuration = {});
    ~SpiceKernelManager() override;

    void load(const SpiceKernelSpec& kernel) override;
    void unload(const SpiceKernelSpec& kernel) override;
    [[nodiscard]] bool isLoaded(const SpiceKernelSpec& kernel) const override;
    [[nodiscard]] bool hasCoverage(int naifId, double ephemerisTime) const override;
    [[nodiscard]] std::optional<SpiceCoverage> coverage(int naifId) const override;
    [[nodiscard]] std::vector<SpiceKernelProvenance> provenance() const override;

    void loadConfiguredKernels();
    void unloadConfiguredKernels();
    [[nodiscard]] const SpiceKernelConfiguration& configuration() const noexcept;

private:
    SpiceKernelConfiguration configuration_;
    std::vector<SpiceKernelProvenance> loaded_;
};

[[nodiscard]] int spiceKernelCategoryCode(SpiceKernelCategory category) noexcept;

} // namespace solarium::ephemeris

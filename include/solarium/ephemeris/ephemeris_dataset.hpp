#pragma once

#include "solarium/ephemeris/canonical_state.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace solarium::ephemeris {

struct EphemerisDatasetKey {
    std::string provider;
    std::string dataset;
    BodyId body;
    std::optional<BodyId> center;
    reference::ReferenceFrame frame;
    TimeScale timeScale;
    time::JulianDate rangeStart;
    time::JulianDate rangeEnd;
    double resolutionSeconds = 0.0;

    [[nodiscard]] bool operator==(const EphemerisDatasetKey&) const noexcept = default;
};

class HermiteInterpolator {
public:
    [[nodiscard]] static CanonicalState interpolate(
        const CanonicalState& first,
        const CanonicalState& second,
        time::JulianDate epoch
    );
};

class EphemerisDataset {
public:
    explicit EphemerisDataset(EphemerisDatasetKey key);

    void addSample(CanonicalState state);

    [[nodiscard]] CanonicalState stateAt(
        BodyId body,
        time::JulianDate epoch
    ) const;

    [[nodiscard]] std::size_t sampleCount() const noexcept;
    [[nodiscard]] const EphemerisDatasetKey& key() const noexcept;
    [[nodiscard]] bool covers(time::JulianDate epoch) const noexcept;

private:
    EphemerisDatasetKey key_;
    std::vector<CanonicalState> samples_;
};

class EphemerisDatasetCache {
public:
    virtual ~EphemerisDatasetCache() = default;

    [[nodiscard]] virtual std::shared_ptr<const EphemerisDataset> find(
        const EphemerisDatasetKey& key
    ) const = 0;

    virtual void store(std::shared_ptr<const EphemerisDataset> dataset) = 0;
};

class InMemoryEphemerisDatasetCache final : public EphemerisDatasetCache {
public:
    explicit InMemoryEphemerisDatasetCache(std::size_t capacity = 8);

    [[nodiscard]] std::shared_ptr<const EphemerisDataset> find(
        const EphemerisDatasetKey& key
    ) const override;

    void store(std::shared_ptr<const EphemerisDataset> dataset) override;

private:
    std::size_t capacity_;
    std::vector<std::shared_ptr<const EphemerisDataset>> datasets_;
};

} // namespace solarium::ephemeris

#pragma once

#include "solarium/ephemeris/canonical_state.hpp"

#include <optional>
#include <vector>

namespace solarium::ephemeris {

struct StateCacheKey {
    BodyId body;
    time::JulianDate epoch;
    std::optional<BodyId> center;
    reference::ReferenceFrame frame;
    TimeScale timeScale;

    [[nodiscard]] bool operator==(const StateCacheKey&) const noexcept = default;
};

class StateCache {
public:
    virtual ~StateCache() = default;

    [[nodiscard]] virtual std::optional<CanonicalState> find(
        const StateCacheKey& key
    ) const = 0;

    virtual void store(const StateCacheKey& key, CanonicalState state) = 0;
};

class InMemoryStateCache final : public StateCache {
public:
    explicit InMemoryStateCache(std::size_t capacity = 64);
    ~InMemoryStateCache() override;

    [[nodiscard]] std::optional<CanonicalState> find(
        const StateCacheKey& key
    ) const override;

    void store(const StateCacheKey& key, CanonicalState state) override;

private:
    struct Entry {
        StateCacheKey key;
        CanonicalState state;
    };

    std::size_t capacity_;
    std::vector<Entry> entries_;
};

} // namespace solarium::ephemeris

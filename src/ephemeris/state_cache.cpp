#include "solarium/ephemeris/state_cache.hpp"

#include <algorithm>
#include <utility>

namespace solarium::ephemeris {
namespace {

bool sameKey(const StateCacheKey& left, const StateCacheKey& right) noexcept {
    return left.body == right.body &&
        left.epoch.value() == right.epoch.value() &&
        left.center == right.center &&
        left.frame == right.frame &&
        left.timeScale == right.timeScale;
}

} // namespace

InMemoryStateCache::InMemoryStateCache(std::size_t capacity)
    : capacity_(capacity), entries_{} {}

InMemoryStateCache::~InMemoryStateCache() = default;

std::optional<CanonicalState> InMemoryStateCache::find(
    const StateCacheKey& key
) const {
    const auto iterator = std::find_if(
        entries_.begin(),
        entries_.end(),
        [&key](const Entry& entry) { return sameKey(entry.key, key); }
    );
    if (iterator == entries_.end()) {
        return std::nullopt;
    }
    return iterator->state;
}

void InMemoryStateCache::store(const StateCacheKey& key, CanonicalState state) {
    if (capacity_ == 0) {
        return;
    }

    const auto iterator = std::find_if(
        entries_.begin(),
        entries_.end(),
        [&key](const Entry& entry) { return sameKey(entry.key, key); }
    );
    if (iterator != entries_.end()) {
        iterator->state = std::move(state);
        return;
    }

    if (entries_.size() == capacity_) {
        entries_.erase(entries_.begin());
    }
    entries_.push_back(Entry{key, std::move(state)});
}

} // namespace solarium::ephemeris

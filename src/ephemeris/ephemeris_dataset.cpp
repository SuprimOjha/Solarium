#include "solarium/ephemeris/ephemeris_dataset.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace solarium::ephemeris {
namespace {

constexpr double SecondsPerDay = 86'400.0;

void validateCompatible(
    const CanonicalState& state,
    const EphemerisDatasetKey& key
) {
    if (state.body() != key.body || state.frame() != key.frame ||
        state.timeScale() != key.timeScale || state.source().center != key.center ||
        state.source().provider != key.provider || state.source().dataset != key.dataset) {
        throw EphemerisError(
            EphemerisErrorCode::InvalidState,
            "ephemeris sample metadata does not match dataset key"
        );
    }
}

} // namespace

CanonicalState HermiteInterpolator::interpolate(
    const CanonicalState& first,
    const CanonicalState& second,
    time::JulianDate epoch
) {
    const double intervalDays = second.epoch() - first.epoch();
    const double intervalSeconds = intervalDays * SecondsPerDay;
    if (intervalSeconds <= 0.0 || epoch.value() < first.epoch().value() ||
        epoch.value() > second.epoch().value()) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid Hermite interpolation interval");
    }
    const double elapsed = (epoch - first.epoch()) * SecondsPerDay;
    const double t = elapsed / intervalSeconds;
    const double t2 = t * t;
    const double t3 = t2 * t;
    const double h00 = 2.0 * t3 - 3.0 * t2 + 1.0;
    const double h10 = t3 - 2.0 * t2 + t;
    const double h01 = -2.0 * t3 + 3.0 * t2;
    const double h11 = t3 - t2;
    const double dh00 = 6.0 * t2 - 6.0 * t;
    const double dh10 = 3.0 * t2 - 4.0 * t + 1.0;
    const double dh01 = -6.0 * t2 + 6.0 * t;
    const double dh11 = 3.0 * t2 - 2.0 * t;

    const auto& p0 = first.position().meters;
    const auto& p1 = second.position().meters;
    const auto& v0 = first.velocity().metersPerSecond;
    const auto& v1 = second.velocity().metersPerSecond;
    const math::Vec3 position =
        p0 * h00 + v0 * (h10 * intervalSeconds) +
        p1 * h01 + v1 * (h11 * intervalSeconds);
    const math::Vec3 velocity =
        p0 * (dh00 / intervalSeconds) + v0 * dh10 +
        p1 * (dh01 / intervalSeconds) + v1 * dh11;

    SourceMetadata source = first.source();
    source.epoch = epoch;
    source.interpolation = InterpolationStatus::Interpolated;
    return CanonicalState(
        first.body(), Position(position), Velocity(velocity), epoch,
        first.timeScale(), first.frame(), std::move(source)
    );
}

EphemerisDataset::EphemerisDataset(EphemerisDatasetKey key)
    : key_(std::move(key)), samples_{} {
    if (key_.provider.empty() || key_.dataset.empty() ||
        key_.rangeStart.value() > key_.rangeEnd.value() ||
        key_.resolutionSeconds < 0.0 || !std::isfinite(key_.resolutionSeconds)) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid ephemeris dataset key");
    }
}

void EphemerisDataset::addSample(CanonicalState state) {
    validateCompatible(state, key_);
    if (!covers(state.epoch())) {
        throw EphemerisError(EphemerisErrorCode::EpochOutsideCoverage, "sample is outside dataset range");
    }
    samples_.push_back(std::move(state));
    std::sort(
        samples_.begin(), samples_.end(),
        [](const CanonicalState& left, const CanonicalState& right) {
            return left.epoch().value() < right.epoch().value();
        }
    );
}

CanonicalState EphemerisDataset::stateAt(
    BodyId body,
    time::JulianDate epoch
) const {
    if (body != key_.body || samples_.empty() || !covers(epoch)) {
        throw EphemerisError(EphemerisErrorCode::EpochOutsideCoverage, "epoch is outside dataset coverage");
    }
    const auto upper = std::lower_bound(
        samples_.begin(), samples_.end(), epoch.value(),
        [](const CanonicalState& state, double value) {
            return state.epoch().value() < value;
        }
    );
    if (upper != samples_.end() && upper->epoch().value() == epoch.value()) {
        return *upper;
    }
    if (upper == samples_.begin() || upper == samples_.end()) {
        throw EphemerisError(EphemerisErrorCode::EpochOutsideCoverage, "epoch is outside sampled coverage");
    }
    return HermiteInterpolator::interpolate(*(upper - 1), *upper, epoch);
}

std::size_t EphemerisDataset::sampleCount() const noexcept {
    return samples_.size();
}

const EphemerisDatasetKey& EphemerisDataset::key() const noexcept {
    return key_;
}

bool EphemerisDataset::covers(time::JulianDate epoch) const noexcept {
    return epoch.value() >= key_.rangeStart.value() && epoch.value() <= key_.rangeEnd.value();
}

InMemoryEphemerisDatasetCache::InMemoryEphemerisDatasetCache(std::size_t capacity)
    : capacity_(capacity), datasets_{} {}

std::shared_ptr<const EphemerisDataset> InMemoryEphemerisDatasetCache::find(
    const EphemerisDatasetKey& key
) const {
    for (const auto& dataset : datasets_) {
        if (dataset != nullptr && dataset->key() == key) {
            return dataset;
        }
    }
    return nullptr;
}

void InMemoryEphemerisDatasetCache::store(
    std::shared_ptr<const EphemerisDataset> dataset
) {
    if (dataset == nullptr || capacity_ == 0) {
        return;
    }
    datasets_.erase(
        std::remove_if(
            datasets_.begin(), datasets_.end(),
            [&dataset](const auto& value) {
                return value != nullptr && value->key() == dataset->key();
            }
        ),
        datasets_.end()
    );
    if (datasets_.size() == capacity_) {
        datasets_.erase(datasets_.begin());
    }
    datasets_.push_back(std::move(dataset));
}

} // namespace solarium::ephemeris

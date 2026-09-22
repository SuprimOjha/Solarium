#include "solarium/ephemeris/spice_kernel_manager.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <utility>

#if defined(SOLARIUM_HAS_SPICE)
#include <SpiceUsr.h>
#endif

namespace solarium::ephemeris {
namespace {

[[nodiscard]] std::string lowerExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(
        extension.begin(), extension.end(), extension.begin(),
        [](unsigned char character) { return static_cast<char>(std::tolower(character)); }
    );
    return extension;
}

[[nodiscard]] bool validExtension(
    const std::filesystem::path& path,
    SpiceKernelCategory category
) {
    const std::string extension = lowerExtension(path);
    switch (category) {
    case SpiceKernelCategory::SPK:
        return extension == ".bsp";
    case SpiceKernelCategory::PCK:
        return extension == ".bpc" || extension == ".tpc";
    case SpiceKernelCategory::LSK:
        return extension == ".tls";
    case SpiceKernelCategory::FK:
        return extension == ".tf";
    case SpiceKernelCategory::MetaKernel:
        return extension == ".tm";
    }
    return false;
}

[[nodiscard]] std::filesystem::path checkedPath(
    const SpiceKernelConfiguration& configuration,
    const SpiceKernelSpec& kernel
) {
    const std::filesystem::path relative(kernel.relativePath);
    if (relative.empty() || relative.is_absolute() ||
        std::find(relative.begin(), relative.end(), "..") != relative.end() ||
        !validExtension(relative, kernel.category)) {
        throw EphemerisError(
            EphemerisErrorCode::MalformedData,
            "SPICE kernel path or category is invalid: " + kernel.relativePath
        );
    }
    return std::filesystem::path(configuration.kernelDirectory) / relative;
}

#if defined(SOLARIUM_HAS_SPICE)
void throwSpiceFailure(EphemerisErrorCode code, const char* operation) {
    SpiceChar message[1841]{};
    getmsg_c("LONG", static_cast<SpiceInt>(sizeof(message)), message);
    reset_c();
    throw EphemerisError(code, std::string("SPICE ") + operation + " failed: " + message);
}
#endif

} // namespace

int spiceKernelCategoryCode(SpiceKernelCategory category) noexcept {
    switch (category) {
    case SpiceKernelCategory::SPK: return 1;
    case SpiceKernelCategory::PCK: return 2;
    case SpiceKernelCategory::LSK: return 3;
    case SpiceKernelCategory::FK: return 4;
    case SpiceKernelCategory::MetaKernel: return 5;
    }
    return 0;
}

SpiceKernelManager::SpiceKernelManager(SpiceKernelConfiguration configuration)
    : configuration_(std::move(configuration)), loaded_{} {}

SpiceKernelManager::~SpiceKernelManager() {
#if defined(SOLARIUM_HAS_SPICE)
    for (auto iterator = loaded_.rbegin(); iterator != loaded_.rend(); ++iterator) {
        unload_c(iterator->path.c_str());
    }
#endif
}

void SpiceKernelManager::load(const SpiceKernelSpec& kernel) {
    const std::filesystem::path path = checkedPath(configuration_, kernel);
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        throw EphemerisError(
            EphemerisErrorCode::MissingKernel,
            "SPICE kernel is missing: " + path.string()
        );
    }
#if defined(SOLARIUM_HAS_SPICE)
    furnsh_c(path.string().c_str());
    if (failed_c()) {
        throwSpiceFailure(EphemerisErrorCode::ProviderFailure, "kernel load");
    }
    loaded_.push_back(SpiceKernelProvenance{path.string(), kernel.category});
#else
    (void)path;
    throw EphemerisError(
        EphemerisErrorCode::MissingKernel,
        "SPICE support is disabled; configure with SOLARIUM_ENABLE_SPICE=ON"
    );
#endif
}

void SpiceKernelManager::unload(const SpiceKernelSpec& kernel) {
    const std::filesystem::path path = checkedPath(configuration_, kernel);
#if defined(SOLARIUM_HAS_SPICE)
    unload_c(path.string().c_str());
    if (failed_c()) {
        throwSpiceFailure(EphemerisErrorCode::ProviderFailure, "kernel unload");
    }
#endif
    loaded_.erase(
        std::remove_if(
            loaded_.begin(), loaded_.end(),
            [&path](const SpiceKernelProvenance& value) { return value.path == path.string(); }
        ),
        loaded_.end()
    );
}

bool SpiceKernelManager::isLoaded(const SpiceKernelSpec& kernel) const {
    const std::filesystem::path path = checkedPath(configuration_, kernel);
    return std::any_of(
        loaded_.begin(), loaded_.end(),
        [&path](const SpiceKernelProvenance& value) { return value.path == path.string(); }
    );
}

bool SpiceKernelManager::hasCoverage(int naifId, double ephemerisTime) const {
    const auto value = coverage(naifId);
    return value.has_value() && ephemerisTime >= value->startEphemerisTime &&
        ephemerisTime <= value->endEphemerisTime;
}

std::optional<SpiceCoverage> SpiceKernelManager::coverage(int naifId) const {
#if defined(SOLARIUM_HAS_SPICE)
    std::optional<SpiceCoverage> result;
    SpiceInt count = 0;
    ktotal_c("SPK", &count);
    for (SpiceInt index = 0; index < count; ++index) {
        SpiceChar file[256]{};
        SpiceChar type[32]{};
        SpiceChar source[256]{};
        SpiceInt handle = 0;
        SpiceBoolean found = SPICEFALSE;
        kdata_c(index, "SPK", sizeof(file), sizeof(type), sizeof(source), file, type, source, &handle, &found);
        if (!found) {
            continue;
        }
        SPICEDOUBLE_CELL(intervals, 2000);
        spkcov_c(file, static_cast<SpiceInt>(naifId), &intervals);
        const SpiceInt intervalCount = wncard_c(&intervals);
        for (SpiceInt interval = 0; interval < intervalCount; ++interval) {
            SpiceDouble start = 0.0;
            SpiceDouble end = 0.0;
            wnfetd_c(&intervals, interval, &start, &end);
            if (!result.has_value()) {
                result = SpiceCoverage{start, end};
            } else {
                result->startEphemerisTime = std::min(result->startEphemerisTime, start);
                result->endEphemerisTime = std::max(result->endEphemerisTime, end);
            }
        }
    }
    return result;
#else
    (void)naifId;
    return std::nullopt;
#endif
}

std::vector<SpiceKernelProvenance> SpiceKernelManager::provenance() const {
    return loaded_;
}

void SpiceKernelManager::loadConfiguredKernels() {
    if (!configuration_.metaKernel.empty()) {
        load(SpiceKernelSpec{configuration_.metaKernel, SpiceKernelCategory::MetaKernel});
    }
    for (const SpiceKernelSpec& kernel : configuration_.loadedKernels) {
        load(kernel);
    }
}

void SpiceKernelManager::unloadConfiguredKernels() {
    for (auto iterator = configuration_.loadedKernels.rbegin();
         iterator != configuration_.loadedKernels.rend(); ++iterator) {
        if (isLoaded(*iterator)) {
            unload(*iterator);
        }
    }
    if (!configuration_.metaKernel.empty()) {
        const SpiceKernelSpec meta{configuration_.metaKernel, SpiceKernelCategory::MetaKernel};
        if (isLoaded(meta)) {
            unload(meta);
        }
    }
}

const SpiceKernelConfiguration& SpiceKernelManager::configuration() const noexcept {
    return configuration_;
}

} // namespace solarium::ephemeris

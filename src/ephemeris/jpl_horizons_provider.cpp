#include "solarium/ephemeris/jpl_horizons_provider.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace solarium::ephemeris {
namespace {

struct HorizonsBodyMapping {
    BodyId body;
    std::string_view id;
};

constexpr HorizonsBodyMapping bodyMappings[] = {
    {BodyId::Sun, "10"}, {BodyId::Mercury, "199"}, {BodyId::Venus, "299"},
    {BodyId::Earth, "399"}, {BodyId::Moon, "301"}, {BodyId::Mars, "499"},
    {BodyId::Phobos, "401"}, {BodyId::Deimos, "402"}, {BodyId::Jupiter, "599"},
    {BodyId::Io, "501"}, {BodyId::Europa, "502"}, {BodyId::Ganymede, "503"},
    {BodyId::Callisto, "504"}, {BodyId::Saturn, "699"}, {BodyId::Mimas, "601"},
    {BodyId::Enceladus, "602"}, {BodyId::Tethys, "603"}, {BodyId::Dione, "604"},
    {BodyId::Rhea, "605"}, {BodyId::Titan, "606"}, {BodyId::Iapetus, "608"},
    {BodyId::Uranus, "799"}, {BodyId::Miranda, "701"}, {BodyId::Ariel, "702"},
    {BodyId::Umbriel, "703"}, {BodyId::Titania, "704"}, {BodyId::Oberon, "705"},
    {BodyId::Neptune, "899"}, {BodyId::Triton, "801"}
};

[[nodiscard]] std::string urlEncode(std::string_view value) {
    std::ostringstream encoded;
    encoded << std::uppercase << std::hex;
    for (const unsigned char character : value) {
        if ((character >= 'a' && character <= 'z') ||
            (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9') ||
            character == '-' || character == '_' || character == '.' || character == '~') {
            encoded << character;
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0')
                    << static_cast<int>(character);
        }
    }
    return encoded.str();
}

[[nodiscard]] std::string makeEpoch(time::JulianDate epoch) {
    std::ostringstream value;
    value << std::fixed << std::setprecision(9) << epoch.value();
    return value.str();
}

[[nodiscard]] std::string centerId(std::optional<BodyId> center) {
    if (!center.has_value()) {
        return "500@0";
    }
    return "500@" + std::string(horizonsBodyId(*center));
}

[[nodiscard]] std::string makeUrl(
    const JplHorizonsConfiguration& configuration,
    const EphemerisRequest& request
) {
    if (configuration.outputUnits != HorizonsOutputUnits::KilometersAndSeconds) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "unsupported Horizons output units");
    }

    const std::string epoch = makeEpoch(request.epoch);
    const char* timeType = request.timeScale == TimeScale::UTC
        ? "UTC"
        : request.timeScale == TimeScale::TT ? "TT" : "TDB";
    const std::vector<std::pair<std::string, std::string>> parameters = {
        {"COMMAND", "'" + std::string(horizonsBodyId(request.body)) + "'"},
        {"EPHEM_TYPE", "VECTORS"},
        {"CENTER", "'" + centerId(request.center) + "'"},
        {"START_TIME", "'JD " + epoch + "'"},
        {"STOP_TIME", "'JD " + epoch + "'"},
        {"STEP_SIZE", "'1 d'"},
        {"VEC_TABLE", "'2'"},
        {"CSV_FORMAT", "'YES'"},
        {"OBJ_DATA", "'YES'"},
        {"REF_SYSTEM", "'ICRF'"},
        {"REF_PLANE", "'ECLIPTIC'"},
        {"OUT_UNITS", "'KM-S'"},
        {"TIME_TYPE", std::string("'") + timeType + "'"}
    };

    std::ostringstream url;
    url << configuration.apiUrl;
    url << '?';
    for (std::size_t index = 0; index < parameters.size(); ++index) {
        if (index != 0) {
            url << '&';
        }
        url << parameters[index].first << '=' << urlEncode(parameters[index].second);
    }
    return url.str();
}

[[nodiscard]] std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

[[nodiscard]] std::vector<std::string> splitCsv(std::string_view line) {
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;
    for (const char character : line) {
        if (character == '"') {
            quoted = !quoted;
        } else if (character == ',' && !quoted) {
            fields.push_back(trim(std::move(field)));
            field.clear();
        } else {
            field.push_back(character);
        }
    }
    fields.push_back(trim(std::move(field)));
    return fields;
}

[[nodiscard]] double parseDouble(std::string_view text, const char* field) {
    const std::string value(text);
    char* end = nullptr;
    errno = 0;
    const double result = std::strtod(value.c_str(), &end);
    if (errno == ERANGE || end != value.c_str() + value.size() || !std::isfinite(result)) {
        throw EphemerisError(
            EphemerisErrorCode::MalformedData,
            std::string("invalid Horizons ") + field + " value"
        );
    }
    return result;
}

[[nodiscard]] std::string headerValue(std::string_view body, std::string_view label) {
    const auto position = body.find(label);
    if (position == std::string_view::npos) {
        return {};
    }
    const auto start = position + label.size();
    const auto end = body.find('\n', start);
    return trim(std::string(body.substr(start, end == std::string_view::npos ? body.size() - start : end - start)));
}

[[nodiscard]] std::string stateLine(std::string_view body) {
    const auto startMarker = body.find("$$SOE");
    const auto endMarker = body.find("$$EOE", startMarker == std::string_view::npos ? 0 : startMarker);
    if (startMarker == std::string_view::npos || endMarker == std::string_view::npos || endMarker <= startMarker) {
        throw EphemerisError(EphemerisErrorCode::MalformedData, "Horizons response has no complete $$SOE/$$EOE block");
    }
    const auto start = body.find('\n', startMarker);
    const auto dataStart = start == std::string_view::npos ? endMarker : start + 1;
    const auto lineEnd = body.find('\n', dataStart);
    const std::string line = trim(std::string(body.substr(dataStart, lineEnd == std::string_view::npos ? endMarker - dataStart : lineEnd - dataStart)));
    if (line.empty()) {
        throw EphemerisError(EphemerisErrorCode::MalformedData, "Horizons response has an empty state-vector row");
    }
    return line;
}

[[nodiscard]] CanonicalState parseState(
    std::string_view body,
    const EphemerisRequest& request
) {
    if (body.find("API ERROR") != std::string_view::npos || body.find("ERROR") != std::string_view::npos) {
        throw EphemerisError(EphemerisErrorCode::ProviderFailure, "JPL Horizons returned an API error");
    }
    const std::string units = headerValue(body, "Output units:");
    if (units.find("KM-S") == std::string::npos) {
        throw EphemerisError(EphemerisErrorCode::MalformedData, "Horizons response units are not KM-S");
    }
    const std::string frame = headerValue(body, "Reference frame:");
    if (frame.empty() || frame.find("ICRF") == std::string::npos) {
        throw EphemerisError(EphemerisErrorCode::UnsupportedFrame, "Horizons response is not in ICRF");
    }
    const std::string center = headerValue(body, "Center body name:");
    if (center.empty() || (request.center.has_value() &&
        center.find("(" + std::string(horizonsBodyId(*request.center)) + ")") == std::string::npos)) {
        throw EphemerisError(EphemerisErrorCode::MalformedData, "Horizons response center does not match the request");
    }
    const std::string row = stateLine(body);
    const auto fields = splitCsv(row);
    if (fields.size() < 8) {
        throw EphemerisError(EphemerisErrorCode::MalformedData, "Horizons state-vector row has fewer than eight fields");
    }

    const double epochValue = parseDouble(fields[0], "epoch");
    if (std::abs(epochValue - request.epoch.value()) > 1.0e-7) {
        throw EphemerisError(EphemerisErrorCode::EpochOutsideCoverage, "Horizons returned a different epoch");
    }
    const math::Vec3 position{
        parseDouble(fields[2], "X") * 1'000.0,
        parseDouble(fields[3], "Y") * 1'000.0,
        parseDouble(fields[4], "Z") * 1'000.0
    };
    const math::Vec3 velocity{
        parseDouble(fields[5], "VX") * 1'000.0,
        parseDouble(fields[6], "VY") * 1'000.0,
        parseDouble(fields[7], "VZ") * 1'000.0
    };

    SourceMetadata source;
    source.provider = "JPL Horizons";
    source.dataset = "Horizons API state-vector response";
    source.frameName = "ICRF";
    source.frame = request.frame;
    source.center = request.center;
    source.epoch = request.epoch;
    source.timeScale = request.timeScale;
    source.interpolation = InterpolationStatus::Exact;
    return CanonicalState(
        request.body,
        Position(position),
        Velocity(velocity),
        request.epoch,
        request.timeScale,
        request.frame,
        std::move(source)
    );
}

} // namespace

std::string_view horizonsBodyId(BodyId body) noexcept {
    for (const auto& mapping : bodyMappings) {
        if (mapping.body == body) {
            return mapping.id;
        }
    }
    return {};
}

JplHorizonsProvider::JplHorizonsProvider(
    const JplHorizonsConfiguration& configuration,
    std::shared_ptr<HttpClient> httpClient,
    std::shared_ptr<StateCache> cache
)
    : configuration_(configuration), httpClient_(std::move(httpClient)), cache_(std::move(cache)) {
    if (configuration_.apiUrl.empty() || configuration_.userAgent.empty() ||
        !httpClient_ || !cache_) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid JPL Horizons provider configuration");
    }
}

CanonicalState JplHorizonsProvider::getState(
    const EphemerisRequest& request
) const {
    if (horizonsBodyId(request.body).empty()) {
        throw EphemerisError(EphemerisErrorCode::BodyUnavailable, "body has no JPL Horizons mapping");
    }
    if (request.timeScale != TimeScale::TDB && request.timeScale != TimeScale::TT &&
        request.timeScale != TimeScale::UTC) {
        throw EphemerisError(EphemerisErrorCode::UnsupportedTimeScale, "unsupported Horizons time scale");
    }
    if (!std::isfinite(request.epoch.value())) {
        throw EphemerisError(EphemerisErrorCode::InvalidState, "invalid Horizons request epoch");
    }
    switch (request.frame) {
    case reference::ReferenceFrame::Barycentric:
    case reference::ReferenceFrame::Heliocentric:
    case reference::ReferenceFrame::Geocentric:
        break;
    default:
        throw EphemerisError(EphemerisErrorCode::UnsupportedFrame, "unsupported Horizons reference frame");
    }

    const StateCacheKey key{
        request.body, request.epoch, request.center, request.frame, request.timeScale
    };
    if (const auto cached = cache_->find(key); cached.has_value()) {
        return *cached;
    }

    const HttpResponse response = httpClient_->get({
        makeUrl(configuration_, request),
        configuration_.timeout,
        configuration_.userAgent
    });
    if (response.statusCode < 200 || response.statusCode >= 300) {
        throw EphemerisError(
            EphemerisErrorCode::ProviderFailure,
            "JPL Horizons returned HTTP status " + std::to_string(response.statusCode)
        );
    }
    const CanonicalState state = parseState(response.body, request);
    cache_->store(key, state);
    return state;
}

CanonicalState JplHorizonsProvider::getState(
    BodyId body,
    time::JulianDate epoch,
    reference::ReferenceFrame frame
) const {
    return getState(EphemerisRequest{
        body,
        epoch,
        configuration_.defaultCenter,
        frame,
        configuration_.defaultTimeScale
    });
}

CanonicalState JplHorizonsProvider::getState(
    BodyId body,
    time::JulianDate epoch
) const {
    return getState(EphemerisRequest{
        body,
        epoch,
        configuration_.defaultCenter,
        configuration_.defaultFrame,
        configuration_.defaultTimeScale
    });
}

std::string_view JplHorizonsProvider::providerId() const noexcept {
    return "JPL Horizons";
}

const JplHorizonsConfiguration& JplHorizonsProvider::configuration() const noexcept {
    return configuration_;
}

} // namespace solarium::ephemeris

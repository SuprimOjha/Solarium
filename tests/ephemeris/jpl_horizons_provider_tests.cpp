#include "solarium/ephemeris/jpl_horizons_provider.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace {

using namespace solarium;
using ephemeris::BodyId;
using ephemeris::EphemerisError;
using ephemeris::EphemerisErrorCode;
using ephemeris::EphemerisRequest;
using ephemeris::HttpClient;
using ephemeris::HttpRequest;
using ephemeris::HttpResponse;
using ephemeris::JplHorizonsProvider;
using ephemeris::TimeScale;
using reference::ReferenceFrame;
using time::JulianDate;

std::string fixture() {
    std::ifstream file(std::string(SOLARIUM_FIXTURE_DIR) + "/horizons_earth_vectors.txt");
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

class RecordingHttpClient final : public HttpClient {
public:
    explicit RecordingHttpClient(std::string response, long statusCode = 200)
        : response_(std::move(response)), statusCode_(statusCode) {}

    [[nodiscard]] HttpResponse get(const HttpRequest& request) const override {
        ++calls;
        lastRequest = request;
        return {statusCode_, response_};
    }

    mutable int calls = 0;
    mutable HttpRequest lastRequest;

private:
    std::string response_;
    long statusCode_;
};

EphemerisRequest earthRequest() {
    return {
        BodyId::Earth,
        JulianDate(2'451'545.0),
        BodyId::Sun,
        ReferenceFrame::Heliocentric,
        TimeScale::TDB
    };
}

} // namespace

int main() {
    auto client = std::make_shared<RecordingHttpClient>(fixture());
    auto cache = std::make_shared<ephemeris::InMemoryStateCache>();
    JplHorizonsProvider provider({}, client, cache);

    const auto state = provider.getState(earthRequest());
    assert(provider.providerId() == "JPL Horizons");
    assert(state.body() == BodyId::Earth);
    assert(state.position().meters.x == 1'000.0);
    assert(state.position().meters.z == 3'000.0);
    assert(state.velocity().metersPerSecond.y == 5'000.0);
    assert(state.frame() == ReferenceFrame::Heliocentric);
    assert(state.timeScale() == TimeScale::TDB);
    assert(state.source().provider == "JPL Horizons");
    assert(state.source().center == BodyId::Sun);
    assert(client->calls == 1);
    assert(client->lastRequest.url.find("COMMAND=%27199%27") != std::string::npos);
    assert(client->lastRequest.url.find("CENTER=%27500%4010%27") != std::string::npos);
    assert(client->lastRequest.url.find("OUT_UNITS=%27KM-S%27") != std::string::npos);
    assert(client->lastRequest.userAgent == "Solarium/2.0");

    const auto cached = provider.getState(earthRequest());
    assert(cached.position().meters.x == state.position().meters.x);
    assert(client->calls == 1);

    bool malformed = false;
    auto malformedClient = std::make_shared<RecordingHttpClient>("$$SOE\nnot,a,state\n$$EOE\n");
    JplHorizonsProvider malformedProvider({}, malformedClient, std::make_shared<ephemeris::InMemoryStateCache>());
    try {
        malformedProvider.getState(earthRequest());
    } catch (const EphemerisError& error) {
        malformed = error.code() == EphemerisErrorCode::MalformedData;
    }
    assert(malformed);

    bool httpFailure = false;
    auto failedClient = std::make_shared<RecordingHttpClient>("service unavailable", 503);
    JplHorizonsProvider failedProvider({}, failedClient, std::make_shared<ephemeris::InMemoryStateCache>());
    try {
        failedProvider.getState(earthRequest());
    } catch (const EphemerisError& error) {
        httpFailure = error.code() == EphemerisErrorCode::ProviderFailure;
    }
    assert(httpFailure);

    std::cout << "JplHorizonsProviderTests passed.\n";
    return 0;
}

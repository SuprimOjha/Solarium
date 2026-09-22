#include "solarium/ephemeris/http_client.hpp"

#include "solarium/ephemeris/ephemeris_error.hpp"

#include <chrono>
#include <string>

#if defined(SOLARIUM_HAS_CURL)
#include <curl/curl.h>
#endif

namespace solarium::ephemeris {

#if defined(SOLARIUM_HAS_CURL)
namespace {

std::size_t writeBody(char* data, std::size_t size, std::size_t count, void* userData) {
    auto* body = static_cast<std::string*>(userData);
    body->append(data, size * count);
    return size * count;
}

} // namespace
#endif

HttpResponse CurlHttpClient::get(const HttpRequest& request) const {
#if defined(SOLARIUM_HAS_CURL)
    CURL* handle = curl_easy_init();
    if (handle == nullptr) {
        throw EphemerisError(EphemerisErrorCode::ProviderFailure, "could not initialize libcurl");
    }

    std::string body;
    struct curl_slist* headers = nullptr;
    if (!request.userAgent.empty()) {
        headers = curl_slist_append(headers, ("User-Agent: " + request.userAgent).c_str());
    }

    curl_easy_setopt(handle, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 0L);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, static_cast<long>(request.timeout.count()));
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeBody);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 0L);
    const CURLcode result = curl_easy_perform(handle);

    long statusCode = 0;
    if (result == CURLE_OK) {
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &statusCode);
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);

    if (result == CURLE_OPERATION_TIMEDOUT) {
        throw EphemerisError(EphemerisErrorCode::ProviderFailure, "JPL Horizons request timed out");
    }
    if (result != CURLE_OK) {
        throw EphemerisError(
            EphemerisErrorCode::ProviderFailure,
            std::string("JPL Horizons HTTP request failed: ") + curl_easy_strerror(result)
        );
    }
    return HttpResponse{statusCode, std::move(body)};
#else
    (void)request;
    throw EphemerisError(
        EphemerisErrorCode::ProviderFailure,
        "JPL Horizons requires a libcurl-enabled build"
    );
#endif
}

} // namespace solarium::ephemeris

#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace solarium::ephemeris {

struct HttpRequest {
    std::string url;
    std::chrono::milliseconds timeout{10'000};
    std::string userAgent;
};

struct HttpResponse {
    long statusCode = 0;
    std::string body;
};

class HttpClient {
public:
    virtual ~HttpClient() = default;

    [[nodiscard]] virtual HttpResponse get(const HttpRequest& request) const = 0;
};

class CurlHttpClient final : public HttpClient {
public:
    [[nodiscard]] HttpResponse get(const HttpRequest& request) const override;
};

} // namespace solarium::ephemeris

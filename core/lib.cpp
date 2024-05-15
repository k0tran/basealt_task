#include "lib.hpp"

#include <cpr/cpr.h>

ALTAPI::ALTAPI(std::string url) : base_url(url) {}

std::optional<json> ALTAPI::get(const std::string &path) const {
    cpr::Response r = cpr::Get(
        cpr::Url{base_url + path}
    );
    if (r.status_code == 200)
        return json::parse(r.text);
    return {};
}

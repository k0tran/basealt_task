#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <optional>

using json = nlohmann::json;

/// Class for getting json from branch name
class ALTAPI {
public:
    ALTAPI(std::string url);

    std::optional<json> get(const std::string &api) const;
private:
    std::string base_url;
};

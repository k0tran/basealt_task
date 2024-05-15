#pragma once

#include "nlohmann/json_fwd.hpp"
#include <functional>
#include <nlohmann/json.hpp>

#include <string>
#include <optional>

using json = nlohmann::json;
using ojson = nlohmann::ordered_json;

/// Class for getting json from branch name
class ALTAPI {
public:
    explicit ALTAPI(std::string url);

    std::optional<json> get(const std::string &) const;
private:
    std::string base_url;
};

enum class BranchFilter {
    RIGHT,
    LEFT,
    NONE
};

/// Class for parsing branch info
/// Utilizes ordered structure of request
class BranchData {
public:
    explicit BranchData(json);

    json substract(const BranchData &) const;

    json filter(const BranchData &, std::function<BranchFilter(const json &, const json &)>) const;

private:
    ojson data;
};

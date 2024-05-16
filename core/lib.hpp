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

/// Class for parsing branch info
/// Utilizes ordered structure of request
class BranchData {
public:
    enum class TakePkg {
        RIGHT,
        LEFT,
        NONE
    };

  BranchData() = default;
  explicit BranchData(json &&);

  BranchData(const BranchData &) = default;
  BranchData(BranchData &&) = default;
  BranchData &operator=(const BranchData &) = default;
  BranchData &operator=(BranchData &&) = default;

  BranchData operator-(const BranchData &) const;

  BranchData combine(const BranchData &, std::function<TakePkg(const json &, const json &)>) const;

  /// Makes flat json array consuming inner map
  json flatten() const &&;

private:
    std::map<std::pair<std::string, std::string>, json> data;
};

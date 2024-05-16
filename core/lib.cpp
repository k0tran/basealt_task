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

BranchData::BranchData(json &&source) {
    for (auto &&pkg : source["packages"]) {
        json stripped(std::move(pkg));
        std::string arch = stripped["arch"];
        std::string name = stripped["name"];
        stripped.erase("arch");
        stripped.erase("name");

        data.emplace(
            std::make_pair(std::move(arch), std::move(name)),
            stripped
        );
    }
}

BranchData BranchData::operator-(const BranchData &other) const {
    BranchData result;
    for (const auto &entry : data)
        if (other.data.count(entry.first) == 0)
            result.data.insert(entry);
    return std::move(result);
}

BranchData BranchData::combine(const BranchData &other, std::function<TakePkg(const json &, const json &)> cmp) const {
    BranchData result;
    for (const auto &entry : data)
        if (other.data.count(entry.first))
            switch (cmp(entry.second, other.data.at(entry.first))) {
                case TakePkg::LEFT:
                    result.data.insert(entry);
                    break;
                case TakePkg::RIGHT:
                    result.data.emplace(std::make_pair(entry.first, other.data.at(entry.second)));
                    break;
                case TakePkg::NONE:
                    break; // Do nothing
            }
    return std::move(result);
}

json BranchData::flatten() const && {
    json result = json::array();
    for (auto &&entry : data) {
        result.emplace_back(entry.second);
        result.back()["arch"] = std::move(entry.first.first);
        result.back()["name"] = std::move(entry.first.second);
    }
    return std::move(result);
}

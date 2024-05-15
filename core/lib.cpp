#include "lib.hpp"

#include <cpr/cpr.h>
#include <stdexcept>

ALTAPI::ALTAPI(std::string url) : base_url(url) {}

std::optional<json> ALTAPI::get(const std::string &path) const {
    cpr::Response r = cpr::Get(
        cpr::Url{base_url + path}
    );
    if (r.status_code == 200)
        return json::parse(r.text);
    return {};
}

BranchData::BranchData(json source) {
    for (size_t i = 0; i < source["packages"].size(); ++i) {
        json stripped{std::move(source["packages"][i])};
        std::string arch = stripped["arch"];
        std::string name = stripped["name"];
        stripped.erase("arch");
        stripped.erase("name");

        // Arch and name level should be ordered
        if (!data.contains(arch))
            data[arch] = ojson();

        data[arch][name] = std::move(stripped);
    }
}

json BranchData::substract(const BranchData &other) const {
    json result = json::array();

    // Cycle over arch
    for (const auto &it : data.items()) {
        // Check if other branch has this arch
        if (!other.data.contains(it.key()))
            continue;

        result.push_back(it.key());
        // Cycle over packets in one arch
        for (const auto &pkg : it.value().items())
            if (other.data[it.key()].contains(pkg.key())) {
                // Copy package and store it with name
                json package{pkg.value()};
                package["name"] = pkg.key();
                result[it.key()].push_back(std::move(package));
            }
    }

    return result;
}

json BranchData::filter(const BranchData &other, std::function<BranchFilter(const json &, const json &)> cmp) const {
    json result = json::array();

    // Cycle over arch
    for (const auto &it : data.items()) {
        // Check if other branch has this arch
        if (!other.data.contains(it.key()))
            continue;


        // Cycle over packets in one arch
        for (const auto &pkg : it.value().items()) {
            // Check if other branch has this package
            if (!other.data[it.key()].contains(pkg.key()))
                continue;

            const json &right = other.data[it.key()][pkg.key()];
            switch (cmp(pkg.value(), right)) {
                case BranchFilter::LEFT: {
                    json package{pkg.value()};
                    package["name"] = pkg.key();
                    result.push_back(std::move(package));
                    break;
                }
                case BranchFilter::RIGHT: {
                    json package{right};
                    package["name"] = pkg.key();
                    result.push_back(std::move(package));
                    break;
                }
                case BranchFilter::NONE: { break; } // Do nothing
            }

        }
    }

    return result;
}

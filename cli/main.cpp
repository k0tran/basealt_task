// Local library
#include <cstdlib>
#include <exception>
#include <lib.hpp>

#include <rpm/rpmlib.h>
#include <rpm/header.h>
#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

BranchData from_file(const std::string &filename) {
    std::ifstream f{filename};
    return BranchData(std::move(json::parse(f)));
}

BranchData from_web(ALTAPI &api, const std::string &url) {
    auto res = api.get(url);
    if (!res.has_value())
        throw std::runtime_error("Could not reach API");
    return BranchData(std::move(res.value()));
}

int main(int argc, char **argv) {
    CLI::App app{"CLI tool for comparing packages in ALT branches"};

    std::string b1_name, b1_file;
    auto branch1_opt = app.add_option_group("First branch");
    branch1_opt->add_option("-1,--b1", b1_name, "Get branch by it's name from the API");
    branch1_opt->add_option("--file1", b1_file, "Load branch data from file")->check(CLI::ExistingFile);
    branch1_opt->require_option(1);

    std::string b2_name, b2_file;
    auto branch2_opt = app.add_option_group("Second branch");
    branch2_opt->add_option("-2,--b2", b2_name, "Specify second branch name");
    branch2_opt->add_option("--file2", b2_file, "Load branch data from file")->check(CLI::ExistingFile);
    branch2_opt->require_option(1);

    std::string base_url;
    app.add_option("API base URL", base_url)->default_val("rdb.altlinux.org/api/");

    std::string branch_prefix;
    app.add_option("Branch prefix", branch_prefix)->default_val("/export/branch_binary_packages/");

    bool dump_readable;
    app.add_flag("-r", dump_readable)->default_val(false);

    CLI11_PARSE(app, argc, argv);

    ALTAPI api{base_url};

    BranchData b1, b2;
    try {
        b1 = b1_name.empty() ? from_file(b1_file) : from_web(api, branch_prefix + b1_name);
        b2 = b2_name.empty() ? from_file(b2_file) : from_web(api, branch_prefix + b2_name);
    } catch (const std::exception &e) {
        std::cerr << "Failed to get branch data" << std::endl;
        return EXIT_FAILURE;
    }

    json result({});
    result["b1_vs_b2"] = (b1 - b2).flatten();
    result["b2_vs_b2"] = (b2 - b1).flatten();
    result["versions"] = b1.combine(b2, [](auto p1, auto p2) {
        std::stringstream ss1, ss2;
        ss1 << p1["version"] << '-' << p1["release"];
        ss2 << p2["version"] << '-' << p2["release"];

        int result = rpmvercmp(ss1.str().c_str(), ss2.str().c_str());
        if (result > 0)
            return BranchData::TakePkg::LEFT;
        else
            return BranchData::TakePkg::NONE;
    }).flatten();

    std::cout << (dump_readable ? result.dump(4) : result.dump()) << std::endl;

    return EXIT_SUCCESS;
}

// Local library
#include <lib.hpp>

#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char **argv) {
    CLI::App app{"CLI tool for comparing packages in ALT branches"};

    std::string branch;
    app.add_option("-b,--branch", branch, "Specify branch to dump packages from")->required();

    CLI11_PARSE(app, argc, argv);

    ALTAPI api{"rdb.altlinux.org/api/"};

    auto result = api.get("/export/branch_binary_packages/" + branch);
    if (!result.has_value()) {
        std::cerr << "Unreachable API. Make sure that API is up and branch name is correct" << std::endl;
        return 1;
    }
    std::cout << "Found " << result.value()["packages"].size() << " packages" << std::endl;

    return 0;
}

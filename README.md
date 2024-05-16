# BaseALT test assigment

Lib and CLI tool for comparing two ALT Linux branch packages. Produces JSON that contains:

-   All packages that are present in first branch and not present in second (field `b1_vs_b2`);
-   All packages that are present in second branch ant not present in first (field `b2_vs_b1`);
-   All packages from first branch that have bigger version-release (**NOT WORKING**)

Usage:

```
CLI tool for comparing packages in ALT branches
Usage: ./cli [OPTIONS] [API base URL] [Branch prefix]

Positionals:
  API base URL TEXT [rdb.altlinux.org/api/]
  Branch prefix TEXT [/export/branch_binary_packages/]

Options:
  -h,--help                   Print this help message and exit
  -r [0]
[Option Group: First branch]

  [Exactly 1 of the following options is required]
  Options:
    -1,--b1 TEXT                Get branch by it's name from the API
    --file1 TEXT:FILE           Load branch data from file
[Option Group: Second branch]

  [Exactly 1 of the following options is required]
  Options:
    -2,--b2 TEXT                Specify second branch name
    --file2 TEXT:FILE           Load branch data from file
```

## How to build

There is `CMakeLists.txt` in the project root. All dependencies downloaded using `FetchContent`:

-   [cpr](https://github.com/libcpr/cpr);
-   [nlohmann_json](https://github.com/nlohmann/json);
-   [CLI11](https://github.com/CLIUtils/CLI11);
-   [~~semver~~](https://github.com/Neargye/semver);

For linux command may look like something like this:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cd build
make
```

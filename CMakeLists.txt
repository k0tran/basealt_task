cmake_minimum_required(VERSION 3.21)
project(basealt_task)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0135 NEW)

set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(DEFAULT_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(FetchContent)

FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

# Find the RPM library and headers
find_path(RPM_INCLUDE_DIR NAMES rpm/rpmlib.h)
find_library(RPM_LIBRARY NAMES rpm)
find_library(RPMIO_LIBRARY NAMES rpmio)

if (RPM_INCLUDE_DIR AND RPM_LIBRARY AND RPMIO_LIBRARY)
    message(STATUS "Found RPM: ${RPM_LIBRARY}, RPMIO: ${RPMIO_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find RPM library or headers")
endif()

add_library(core core/lib.cpp)
target_link_libraries(core PRIVATE cpr::cpr)
target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)

add_executable(cli cli/main.cpp)
target_include_directories(cli PRIVATE ${RPM_INCLUDE_DIR})
target_link_libraries(cli PRIVATE core)
target_include_directories(cli PRIVATE core)
target_link_libraries(cli PRIVATE CLI11::CLI11)
target_link_libraries(cli PRIVATE ${RPM_LIBRARY} ${RPMIO_LIBRARY})

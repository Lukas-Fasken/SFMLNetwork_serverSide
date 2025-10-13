cmake_minimum_required(VERSION 3.26)
project(ServerSide LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# Collect sources
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.cpp
    includes/*.cpp
)

add_executable(main ${PROJECT_SOURCES})

# Add include directory for headers
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/includes)

# Set C++ standard
target_compile_features(main PRIVATE cxx_std_20)

# Link SFML
target_link_libraries(main PRIVATE SFML::Graphics nlohmann_json::nlohmann_json)

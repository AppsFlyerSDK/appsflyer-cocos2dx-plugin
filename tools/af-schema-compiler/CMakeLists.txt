cmake_minimum_required(VERSION 3.16)
project(af_schema_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(af-schema-compiler
    src/main.cpp
    src/emit.cpp
    src/schema_model.cpp
)

target_link_libraries(af-schema-compiler PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(af-schema-compiler PRIVATE src)

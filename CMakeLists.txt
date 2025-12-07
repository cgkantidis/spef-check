cmake_minimum_required(VERSION 3.20..4.2.0)

project(spef-check
  VERSION 0.0.1
  DESCRIPTION "A tool to parse and check the correctness of a SPEF file"
  LANGUAGES CXX)

# generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(functions.cmake REQUIRED)

include(FetchContent)
FetchContent_Declare(
  pegtl
  GIT_REPOSITORY https://github.com/taocpp/PEGTL.git
  GIT_TAG        be527327653e94b02e711f7eff59285ad13e1db0  # 3.2.8
  OVERRIDE_FIND_PACKAGE
)
find_package(pegtl REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)

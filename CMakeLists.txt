cmake_minimum_required(VERSION 3.27)
project(cypher VERSION 0.0.1 LANGUAGES C)
enable_testing()
set(CMAKE_C_STANDARD 11)

# Global compiler settings.
# Enable warnings as errors
# Set platform specific settings define Windows/Other variables
if(MSVC)
  add_compile_options(/W4 /WX)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
  # Add debug flags for debug build
  string(APPEND CMAKE_C_FLAGS_DEBUG_INIT " -Og -ggdb3")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(oracles)

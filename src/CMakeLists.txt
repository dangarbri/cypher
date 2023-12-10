cmake_minimum_required(VERSION 3.27)
project(cypher VERSION 0.0.1 LANGUAGES C)
enable_testing()

add_executable(cypher
  main.c
  cli/subcommand.c
)

# Test Section
add_executable(test_subcommand
  ../test/test_subcommand.c
  cli/subcommand.c
)
add_executable(test_parsers
  ../test/test_parsers.c
  parsers/hex.c
)

add_test(NAME Subcommands COMMAND test_subcommand)
add_test(NAME Parsers COMMAND test_parsers)

# Eventually will need to add openssl to this.
# target_link_libraries()

# Make sure we are using C17
set_property(TARGET cypher PROPERTY C_STANDARD 17)

# Add debug flags for debug build
string(APPEND CMAKE_C_FLAGS_DEBUG_INIT " -Og -ggdb3")

# Enable warnings
if(MSVC)
  target_compile_options(cypher PRIVATE /W4 /WX)
else()
  target_compile_options(cypher PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
cmake_minimum_required(VERSION 3.27)
project(cypher VERSION 0.0.1 LANGUAGES C)
enable_testing()

add_executable(cypher
  main.c
  cli/argtype.c
  cli/subcommand.c
  commands/base64_cli.c
  operations/xor.c
  parsers/base64.c
  parsers/hex.c
)
target_include_directories(cypher PUBLIC
  ../src
)
target_link_libraries(cypher
  LINK_PRIVATE ssl
  LINK_PRIVATE crypto
)

# Test Section
add_executable(test_subcommand
  ../test/test_subcommand.c
  cli/subcommand.c
)

add_executable(test_argtype
  ../test/test_argtype.c
  parsers/hex.c
  parsers/base64.c
  cli/argtype.c
)
target_include_directories(test_argtype PUBLIC
  ../src
)
target_link_libraries(test_argtype
  LINK_PRIVATE ssl
  LINK_PRIVATE crypto
)

add_executable(test_base64
  ../test/test_base64.c
  parsers/base64.c
)
target_link_libraries(test_base64
  LINK_PRIVATE ssl
  LINK_PRIVATE crypto
)
add_executable(test_parsers
  ../test/test_parsers.c
  parsers/hex.c
)
add_executable(test_xor
  ../test/test_xor.c
  operations/xor.c
)

add_test(NAME Subcommands COMMAND test_subcommand)
add_test(NAME Parsers COMMAND test_parsers)
add_test(NAME Base64 COMMAND test_base64)
add_test(NAME Argtype COMMAND test_argtype)
add_test(NAME xor COMMAND test_xor)

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
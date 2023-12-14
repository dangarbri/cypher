cmake_minimum_required(VERSION 3.27)
project(cypher VERSION 0.0.1 LANGUAGES C)
enable_testing()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Enable warnings as errors, define Windows/Other variables
if(MSVC)
  add_compile_options(/W4 /WX)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  include_directories("C:\\Program Files\\OpenSSL\\include")
  link_directories("C:\\Program Files\\OpenSSL\\lib")
  set(Openssl_LIBRARIES libssl libcrypto)
else()
  # Add debug flags for debug build
  string(APPEND CMAKE_C_FLAGS_DEBUG_INIT " -Og -ggdb3")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  set(Openssl_LIBRARIES ssl crypto)
endif()

add_executable(cypher
  main.c
  analyzers/english.c
  analyzers/sbx.c
  cli/argtype.c
  cli/subcommand.c
  crackers/cracksbx.c
  crackers/crackrbx.c
  commands/analyze.c
  commands/base64_cli.c
  commands/crack.c
  commands/decrypt.c
  commands/print.c
  commands/xor_cli.c
  operations/aes.c
  operations/hamming.c
  operations/rank.c
  operations/xor.c
  parsers/base64.c
  parsers/hex.c
  threading/pool.c
  types/buffer.c
)
install(TARGETS cypher)

include_directories(
  ../src
)
target_link_libraries(cypher
  LINK_PRIVATE ${Openssl_LIBRARIES}
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
  types/buffer.c
)
target_include_directories(test_argtype PUBLIC
  ../src
)
target_link_libraries(test_argtype
  LINK_PRIVATE ${Openssl_LIBRARIES}
)

add_executable(test_base64
  ../test/test_base64.c
  parsers/base64.c
)
target_link_libraries(test_base64
  LINK_PRIVATE ${Openssl_LIBRARIES}
)
add_executable(test_parsers
  ../test/test_parsers.c
  parsers/hex.c
  types/buffer.c
)
add_executable(test_xor
  ../test/test_xor.c
  cli/argtype.c
  parsers/base64.c
  parsers/hex.c
  operations/xor.c
  types/buffer.c
)
target_link_libraries(test_xor
  LINK_PRIVATE ${Openssl_LIBRARIES}
)
add_executable(test_sbxor
  ../test/test_sbxor.c
  operations/xor.c
  types/buffer.c
)
add_executable(test_buffer
  ../test/test_buffer.c
  types/buffer.c
)
add_executable(test_english
  ../test/test_english.c
  analyzers/english.c
  types/buffer.c
)
add_executable(test_cracksbx
  ../test/test_cracksbx.c
  parsers/hex.c
  crackers/cracksbx.c
  analyzers/english.c
  operations/rank.c
  operations/xor.c
  threading/pool.c
  types/buffer.c
)
add_executable(test_threadpool
  ../test/test_threadpool.c
  threading/pool.c
)
add_executable(test_hamming
  ../test/test_hamming.c
  operations/hamming.c
)
add_executable(test_rank
  ../test/test_rank.c
  operations/rank.c
)
add_executable(test_crackrbx
  ../test/test_crackrbx.c
  analyzers/english.c
  crackers/crackrbx.c
  crackers/cracksbx.c
  operations/hamming.c
  operations/rank.c
  operations/xor.c
  parsers/hex.c
  threading/pool.c
  types/buffer.c
)
add_executable(test_aes
  ../test/test_aes.c
  cli/argtype.c
  parsers/hex.c
  parsers/base64.c
  operations/aes.c
  types/buffer.c
)
target_link_libraries(test_aes
  LINK_PRIVATE ${Openssl_LIBRARIES}
)

add_test(NAME Subcommands COMMAND test_subcommand)
add_test(NAME Parsers COMMAND test_parsers)
add_test(NAME Base64 COMMAND test_base64)
add_test(NAME Argtype COMMAND test_argtype)
add_test(NAME xor COMMAND test_xor)
add_test(NAME sbxor COMMAND test_sbxor)
add_test(NAME buffer COMMAND test_buffer)
add_test(NAME EnglishAnalyzer COMMAND test_english)
add_test(NAME CrackSBX COMMAND test_cracksbx)
add_test(NAME CrackRBX COMMAND test_crackrbx)
add_test(NAME Rank COMMAND test_rank)
add_test(NAME Hamming COMMAND test_hamming)
add_test(NAME AES COMMAND test_aes)
add_test(NAME ThreadPool COMMAND test_threadpool)

# Eventually will need to add openssl to this.
# target_link_libraries()

# Make sure we are using C17
set_property(TARGET cypher PROPERTY C_STANDARD 17)

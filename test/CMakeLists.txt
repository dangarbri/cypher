# Copy required test data to the build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DNDEBUG=0)
include_directories(../src)
link_libraries(${OPENSSL_LIBRARIES})

set(ARGTYPE_SOURCES
../src/parsers/hex.c
../src/parsers/base64.c
../src/cli/system.c
../src/cli/argtype.c
../src/types/buffer.c
)

# Test Section
add_executable(test_subcommand
  test_subcommand.c
  ../src/cli/subcommand.c
)
add_executable(test_argtype
  test_argtype.c
  ${ARGTYPE_SOURCES}
)
add_executable(test_argtype_stdin
  test_argtype_stdin.c
  ${ARGTYPE_SOURCES}
)
add_executable(test_argtype_stdin_b64
  test_argtype_stdin_b64.c
  ${ARGTYPE_SOURCES}
)
add_executable(test_argtype_stdin_hex
  test_argtype_stdin_hex.c
  ${ARGTYPE_SOURCES}
)
add_executable(test_base64
  test_base64.c
  ../src/parsers/base64.c
  ../src/types/buffer.c
)
add_executable(test_parsers
  test_parsers.c
  ../src/parsers/hex.c
  ../src/types/buffer.c
)
add_executable(test_sbxor
  test_sbxor.c
  ../src/operations/xor.c
  ../src/types/buffer.c
)
add_executable(test_buffer
  test_buffer.c
  ../src/types/buffer.c
)
add_executable(test_english
  test_english.c
  ../src/analyzers/english.c
  ../src/types/buffer.c
)
add_executable(test_cracksbx
  test_cracksbx.c
  ../src/parsers/hex.c
  ../src/crackers/cracksbx.c
  ../src/analyzers/english.c
  ../src/operations/rank.c
  ../src/operations/xor.c
  ../src/threading/pool.c
  ../src/types/buffer.c
)
add_executable(test_threadpool
  test_threadpool.c
  ../src/threading/pool.c
)
add_executable(test_hamming
  test_hamming.c
  ../src/operations/hamming.c
)
add_executable(test_rank
  test_rank.c
  ../src/operations/rank.c
)
add_executable(test_crackrbx
  test_crackrbx.c
  ../src/analyzers/english.c
  ../src/crackers/crackrbx.c
  ../src/crackers/cracksbx.c
  ../src/operations/hamming.c
  ../src/operations/rank.c
  ../src/operations/xor.c
  ../src/parsers/hex.c
  ../src/threading/pool.c
  ../src/types/buffer.c
)

link_libraries(${Openssl_LIBRARIES})
add_executable(test_aes
  test_aes.c
  ${ARGTYPE_SOURCES}
  ../src/operations/aes.c
  ../src/operations/pad.c
  ../src/operations/xor.c
)
add_executable(test_pad
  test_pad.c
  ${ARGTYPE_SOURCES}
  ../src/operations/pad.c
)
add_executable(test_xor
  test_xor.c
  ${ARGTYPE_SOURCES}
  ../src/operations/xor.c
)
add_executable(test_streplace
  test_streplace.c
  ../src/cli/system.c
  ../src/strings/replace.c
  ../src/types/buffer.c
)

add_test(NAME Subcommands COMMAND test_subcommand)
add_test(NAME Parsers COMMAND test_parsers)
add_test(NAME Base64 COMMAND test_base64)
add_test(NAME Argtype COMMAND test_argtype)
if(NOT MSVC)
add_test(NAME Argtype_stdin COMMAND bash -c "echo -n 'hello' | ./test_argtype_stdin")
add_test(NAME Argtype_stdin_b64 COMMAND bash -c "echo -n \"aGVsbG8=\" | ./test_argtype_stdin_b64")
add_test(NAME Argtype_stdin_b64_newline COMMAND bash -c "echo \"aGVsbG8=\" | ./test_argtype_stdin_b64")
add_test(NAME Argtype_stdin_hex COMMAND bash -c "echo -n \"68656c6c6f\" | ./test_argtype_stdin_hex")
else()
# TODO: How to run these on windows?
endif()
add_test(NAME xor COMMAND test_xor)
add_test(NAME sbxor COMMAND test_sbxor)
add_test(NAME buffer COMMAND test_buffer)
add_test(NAME EnglishAnalyzer COMMAND test_english)
add_test(NAME CrackSBX COMMAND test_cracksbx)
add_test(NAME CrackRBX COMMAND test_crackrbx)
add_test(NAME Rank COMMAND test_rank)
add_test(NAME Hamming COMMAND test_hamming)
add_test(NAME AES COMMAND test_aes)
add_test(NAME Pad COMMAND test_pad)
add_test(NAME streplace COMMAND test_streplace)
add_test(NAME ThreadPool COMMAND test_threadpool)
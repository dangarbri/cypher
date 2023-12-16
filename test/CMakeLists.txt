# Copy required test data to the build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DNDEBUG=0)
include_directories(../src)
link_libraries(${Openssl_LIBRARIES})
# Test Section
add_executable(test_subcommand
  test_subcommand.c
  ../src/cli/subcommand.c
)
add_executable(test_argtype
  test_argtype.c
  ../src/parsers/hex.c
  ../src/parsers/base64.c
  ../src/cli/argtype.c
  ../src/types/buffer.c
)
add_executable(test_base64
  test_base64.c
  ../src/parsers/base64.c
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
if(!MSVC)
add_executable(test_threadpool
  test_threadpool.c
  ../src/threading/pool.c
)
endif()
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
  ../src/cli/argtype.c
  ../src/parsers/hex.c
  ../src/parsers/base64.c
  ../src/operations/aes.c
  ../src/operations/pad.c
  ../src/operations/xor.c
  ../src/types/buffer.c
)
# target_link_libraries(test_aes
#   LINK_PRIVATE ${Openssl_LIBRARIES}
# )
add_executable(test_pad
  test_pad.c
  ../src/operations/pad.c
  ../src/parsers/hex.c
  ../src/parsers/base64.c
  ../src/cli/argtype.c
  ../src/types/buffer.c
)
add_executable(test_xor
  test_xor.c
  ../src/cli/argtype.c
  ../src/parsers/base64.c
  ../src/parsers/hex.c
  ../src/operations/xor.c
  ../src/types/buffer.c
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
add_test(NAME Pad COMMAND test_pad)
if(!MSVC)
# Windows is still working on their threads.h
add_test(NAME ThreadPool COMMAND test_threadpool)
endif()
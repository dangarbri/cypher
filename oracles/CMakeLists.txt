# Copy required test data to the build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ecb_key
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(../src)
link_libraries(
  ${OPENSSL_LIBRARIES}
)
add_executable(encryption_oracle
  encryption_oracle.c
  ../src/cli/argtype.c
  ../src/operations/aes.c
  ../src/operations/pad.c
  ../src/operations/xor.c
  ../src/parsers/hex.c
  ../src/parsers/base64.c
  ../src/types/buffer.c
)
add_executable(ecb_leak
  ecb_leak.c
  ../src/cli/argtype.c
  ../src/operations/aes.c
  ../src/operations/pad.c
  ../src/operations/xor.c
  ../src/parsers/hex.c
  ../src/parsers/base64.c
  ../src/types/buffer.c
)
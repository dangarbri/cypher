include_directories(../src)
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
target_link_libraries(encryption_oracle
  LINK_PRIVATE ${OPENSSL_LIBRARIES}
)

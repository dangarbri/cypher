#ifndef _AES_H_
#define _AES_H_

#include "types/buffer.h"

/**
 * Processes data through the AES ECB encryption algorithm with the given key.
 * @param key Key buffer
 * @param data Data to encrypt/decrypt
 * @return buffer or NULL on failure. Release with Buffer_Free
 */
struct Buffer* Aes128Ecb_Encrypt(struct Buffer* key, struct Buffer* data);

/**
 * Processes data through the AES ECB decryption algorithm with the given key.
 * @param key Key buffer
 * @param data Data to encrypt/decrypt
 * @return buffer or NULL on failure. Release with Buffer_Free
 */
struct Buffer* Aes128Ecb_Decrypt(struct Buffer* key, struct Buffer* data);

/**
 * @param key Key buffer
 * @param data Data to encrypt/decrypt
 * @return buffer or NULL on failure. Release with Buffer_Free
 */
struct Buffer* Aes128Cbc_Encrypt(struct Buffer* key, struct Buffer* iv, struct Buffer* data);

/**
 * @param key Key buffer
 * @param data Data to encrypt/decrypt
 * @return buffer or NULL on failure. Release with Buffer_Free
 */
struct Buffer* Aes128Cbc_Decrypt(struct Buffer* key, struct Buffer* iv, struct Buffer* data);


#endif
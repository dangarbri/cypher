#include <string.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include "base64.h"

int Base64(char** argv) {
    (void) argv;
    return 0;
}

/**
 * Claims the underlying data from the given bio.
 * Calling BIO_free will free the bio without freeing the data.
 * @param bio BIO to claim data for
 * @param out Data pointer will be written here. You are responsible for freeing this.
 * @return length of the data buffer
 */
long BIO_claim_data(BIO* bio, uint8_t** out) {
    BUF_MEM* bptr;
    uint8_t* data;
    long length;

    length = BIO_get_mem_data(bio, &data);
    BIO_get_mem_ptr(bio, &bptr);
    bptr->data = NULL; /* Tell BUF_MEM to orphan data */
    *out = data;
    return length;
}

struct Base64Data Base64_Encode(uint8_t* data, size_t length) {
    struct Base64Data result = {
        .data = NULL,
        .length = 0,
        .valid = false
    };
    // Create a BIO for storing encoded data
    BIO* encoded = BIO_new(BIO_s_mem());
    if (encoded != NULL) {
        // Instantiate base64 decoder
        BIO* b64 = BIO_new(BIO_f_base64());
        if (b64 != NULL) {
            BIO_push(b64, encoded);
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            // Write data through to encode it
            if (BIO_write(b64, data, length) >= 0) {
                BIO_flush(b64);
                // Extract encoded data from the encoded bio
                result.length = BIO_claim_data(encoded, &result.data);
                result.valid = true;
                BIO_free(b64);
            } else {
                fputs("Failed to encode base64 data", stderr);
            }
        } else {
            fputs("Failled to create base64 encoder", stderr);
        }
        BIO_free(encoded);
    } else {
        fputs("Failed to allocate memory for b64 encoded buffer", stderr);
    }
    return result;
}

struct Base64Data Base64_Decode(char* data) {
    struct Base64Data result = {
        .data = NULL,
        .length = 0,
        .valid = false
    };
    size_t length = strlen(data);
    // Create a BIO around the base64 encoded data
    BIO* encoded = BIO_new_mem_buf(data, length);
    if (encoded != NULL) {
        // Instantiate base64 decoder
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        if (b64 != NULL) {
            // Attach encoded buffer to b64 decoder
            BIO_push(b64, encoded);
            // Read through b64 bio to decode data
            // Allocate enough bytes to handle the output. base64 is pretty close to 1 to 1, excluding padding.
            uint8_t* decoded = malloc(length);
            if (decoded != NULL) {
                int bytes_returned = BIO_read(b64, decoded, length);
                if (bytes_returned != 0) {
                    // Resize final buffer to size of data returned
                    uint8_t* right_size_ptr = realloc(decoded, bytes_returned);
                    if (right_size_ptr != NULL) {
                        result.data = right_size_ptr;
                        result.length = bytes_returned;
                        result.valid = true;
                    } else {
                        free(decoded);
                        fputs("Not enough memory for base64 decoded data", stderr);
                    }
                }
            } else {
                fputs("Failed to allocate memory for base64 decoded data", stderr);
            }
            BIO_free(b64);
        } else {
            fputs("Failed to allocate memory for base64 decoder", stderr);
        }
        BIO_free(encoded);
    } else {
        fputs("Failed to allocate memory for base64 decoding buffer", stderr);
    }

    return result;
}

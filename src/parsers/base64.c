#include <string.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include "base64.h"

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
            if (BIO_write(b64, data, (int) length) >= 0) {
                BIO_flush(b64);
                // Extract encoded data from the encoded bio
                result.length = BIO_claim_data(encoded, &result.data);
                // The result is not null terminated, so let's fix that
                // Allocate another buffer that is 1 byte larger than the length for space for the null byte.
                uint8_t* new_ptr = realloc(result.data, result.length + 1);
                if (new_ptr != NULL) {
                    result.data = new_ptr;
                    result.data[result.length] = '\0';
                    result.valid = true;
                } else {
                    perror("Base64_Encode: ");
                    // Free the original buffer, clear the data and return with valid = false
                    free(result.data);
                    result.data = NULL;
                    result.length = 0;
                }
                BIO_free(b64);
            } else {
                fputs("Failed to encode base64 data\n", stderr);
            }
        } else {
            fputs("Failled to create base64 encoder\n", stderr);
        }
        BIO_free(encoded);
    } else {
        fputs("Failed to allocate memory for b64 encoded buffer\n", stderr);
    }
    return result;
}

struct Base64Data Base64_Decode(char* data) {
    struct Base64Data result = {
        .data = NULL,
        .length = 0,
        .valid = false
    };
    bool no_newlines = strchr((char*) data, '\n') == NULL;
    size_t length = strlen(data);
    // Create a BIO around the base64 encoded data
    BIO* encoded = BIO_new_mem_buf(data, (int) length);
    if (encoded != NULL) {
        // Instantiate base64 decoder
        BIO* b64 = BIO_new(BIO_f_base64());
        if (no_newlines) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }
        if (b64 != NULL) {
            // Attach encoded buffer to b64 decoder
            BIO_push(b64, encoded);
            // Read through b64 bio to decode data
            // Allocate enough bytes to handle the output. base64 is pretty close to 1 to 1, excluding padding.
            uint8_t* decoded = malloc(length);
            if (decoded != NULL) {
                int bytes_returned = BIO_read(b64, decoded, (int) length);
                if (bytes_returned != 0) {
                    // Resize final buffer to size of data returned
                    uint8_t* right_size_ptr = realloc(decoded, bytes_returned);
                    if (right_size_ptr != NULL) {
                        result.data = right_size_ptr;
                        result.length = bytes_returned;
                        result.valid = true;
                    } else {
                        free(decoded);
                        fputs("Not enough memory for base64 decoded data\n", stderr);
                    }
                }
            } else {
                fputs("Failed to allocate memory for base64 decoded data\n", stderr);
            }
            BIO_free(b64);
        } else {
            fputs("Failed to allocate memory for base64 decoder\n", stderr);
        }
        BIO_free(encoded);
    } else {
        fputs("Failed to allocate memory for base64 decoding buffer\n", stderr);
    }

    return result;
}

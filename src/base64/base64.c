#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>

#include "base64.h"

int Base64(char** argv) {
    (void) argv;
    return 0;
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
                        fprintf(stderr, "Not enough memory for base64 decoded data");
                    }
                }
            } else {
                fprintf(stderr, "Failed to allocate memory for base64 decoded data\n");
            }
            BIO_free(b64);
        } else {
            fprintf(stderr, "Failed to allocate memory for base64 decoder\n");
        }
        BIO_free(encoded);
    } else {
        fprintf(stderr, "Failed to allocate memory for base64 decoding buffer\n");
    }

    return result;
}

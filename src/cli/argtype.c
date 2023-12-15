#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argtype.h"
#include "parsers/hex.h"
#include "parsers/base64.h"

/**
 * Parses hex data into a binary arg
 */
struct Buffer* Argtype_Hex(char* hex) {
    struct Buffer* data = Hex_Decode(hex);
    if (data) {
        return data;
    } else {
        fprintf(stderr, "Unable to parse hex: `%s`\n", hex);
    }
    return NULL;
}

struct Buffer* Argtype_Base64(char* base64) {
    struct Base64Data decoded = Base64_Decode(base64);
    if (decoded.valid) {
        struct Buffer *arg = malloc(sizeof(struct Buffer));
        if (arg != NULL) {
            arg->data = decoded.data;
            arg->length = decoded.length;
            return arg;
        } else {
            perror("Argtype_Base64: ");
        }
    } else {
        fprintf(stderr, "Unable to parse base64: `%s`\n", base64);
    }
    return NULL;
}

struct Buffer* Argtype_File(char* fname) {
    if (fname == NULL) { return NULL; }

    struct Buffer* arg = NULL;
    bool failure = true;
    FILE* fp = fopen(fname, "rb");
    if (fp != NULL) {
        // Get size of file
        if(fseek(fp, 0, SEEK_END) == 0) {
            long filesize = ftell(fp);
            if (filesize != -1) {
                // Allocate enough bytes to hold the entire file
                // plus 1 to put a null byte at the end.
                // The null byte is there in case the file is a text file.
                // Without terminating the last byte an operation like strlen could
                // read out of bounds.
                arg = Buffer_New(filesize + 1);
                if (Buffer_IsValid(arg)) {
                    // Read in the entire file
                    rewind(fp);
                    size_t bytes_read = fread(arg->data, 1, filesize, fp);
                    if (bytes_read == (arg->length - 1)) {
                        arg->data[bytes_read] = '\0';
                        // Leave the null byte, but say the length is only the length of the data.
                        arg->length -= 1;
                        // Everying is okay!
                        failure = false;
                    }
                }
            }
        } else {
            perror("Error loading file");
        }

        fclose(fp);
    } else {
        fprintf(stderr, "Failed to open file %s: ", fname);
        perror(NULL);
    }

    // On failure, free up allocated resources
    if (failure) {
        Buffer_Free(arg);
        arg = NULL;
    }
    return arg;
}

struct Buffer* Argtype_B64File(char* fname) {
    if (fname == NULL) { return NULL; }
    // Read in the file
    struct Buffer* file = Argtype_File(fname);
    if (file == NULL) {
        return NULL;
    }
    // Base64 decode the file into a new arg
    struct Base64Data decoded = Base64_Decode((char*) file->data);
    Argtype_Free(file);
    if (decoded.valid) {
        struct Buffer* decoded_file = malloc(sizeof(struct Buffer));
        if (decoded_file != NULL) {
            decoded_file->data = decoded.data;
            decoded_file->length = decoded.length;
            return decoded_file;
        }
    }
    return NULL;
}

struct Buffer* Argtype_String(char* arg) {
    // Buffer functions expect this data to be dynamically allocated.
    // i.e. Buffer_Resize attempts to resize this data pointer, so
    // arg cannot be set to the buffer's data
    size_t size = strlen(arg) + 1;
    struct Buffer* result = Buffer_New(size);
    if (Buffer_IsValid(result)) {
        // copy the input string into the buffer and return the result.
        memcpy(result->data, arg, result->length);
        // the null byte is there, but for strings args we only care about the text.
        result->length -= 1;
        return result;
    } else {
        perror("Argtype_String");
    }
    return NULL;
}

struct Buffer* Argtype_New(char* arg) {
    char* delim = ":";
    char* type = strtok(arg, delim);
    if (type == NULL) {
        return NULL;
    }
    if (strncmp("hex", type, 3) == 0) {
        char* data = strtok(NULL, delim);
        return Argtype_Hex(data);
    } else if (strncmp("base64", type, 6) == 0) {
        char* data = strtok(NULL, delim);
        return Argtype_Base64(data);
    } else if (strncmp("file.b64", type, 8) == 0) {
        char* fname = strtok(NULL, delim);
        return Argtype_B64File(fname);
    } else if (strncmp("file", type, 4) == 0) {
        char* fname = strtok(NULL, delim);
        return Argtype_File(fname);
    } else {
        return Argtype_String(arg);
    }
}

void Argtype_Free(struct Buffer* arg) {
    Buffer_Free(arg);
}
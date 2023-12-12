#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argtype.h"
#include "parsers/hex.h"
#include "parsers/base64.h"

/**
 * Parses hex data into a binary arg
 */
struct Arg* Argtype_Hex(char* hex) {
    struct HexData data = Hex_Decode(hex);
    if (data.valid) {
        struct Arg *arg = malloc(sizeof(struct Arg));
        if (arg != NULL) {
            arg->type = ARGTYPE_BINARY;
            arg->buffer.data = data.data;
            arg->buffer.length = data.length;
            return arg;
        } else {
            perror("Argtype_Hex: ");
        }
    } else {
        fprintf(stderr, "Unable to parse hex: `%s`\n", hex);
    }
    return NULL;
}

struct Arg* Argtype_Base64(char* base64) {
    struct Base64Data decoded = Base64_Decode(base64);
    if (decoded.valid) {
        struct Arg *arg = malloc(sizeof(struct Arg));
        if (arg != NULL) {
            arg->type = ARGTYPE_BINARY;
            arg->buffer.data = decoded.data;
            arg->buffer.length = decoded.length;
            return arg;
        } else {
            perror("Argtype_Base64: ");
        }
    } else {
        fprintf(stderr, "Unable to parse base64: `%s`\n", base64);
    }
    return NULL;
}

struct Arg* Argtype_String(char* arg) {
    struct Arg* result = malloc(sizeof(struct Arg));
    if (result != NULL) {
        result->type = ARGTYPE_STRING;
        result->buffer.data = (uint8_t*) arg;
        result->buffer.length = strlen(arg);
        return result;
    } else {
        perror("Argtype_String: ");
    }
    return NULL;
}

struct Arg* Argtype_New(char* arg) {
    char* delim = ":";
    char* type = strtok(arg, delim);
    if (strncmp("hex", type, 3) == 0) {
        char* data = strtok(NULL, delim);
        return (struct Arg*) Argtype_Hex(data);
    } else if (strncmp("base64", type, 6) == 0) {
        char* data = strtok(NULL, delim);
        return (struct Arg*) Argtype_Base64(data);
    } else {
        return Argtype_String(arg);
    }
}

void Argtype_Free(struct Arg* arg) {
    if (arg->type == ARGTYPE_BINARY) {
        if (arg->buffer.data != NULL) {
            free(arg->buffer.data);
        }
    }
    free(arg);
}
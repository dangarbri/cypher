#include "flags.h"
#include <string.h>

char* FlagExists(char* flag, int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(flag, argv[i]) == 0) {
            return argv[i];
        }
    }
    return NULL;
}
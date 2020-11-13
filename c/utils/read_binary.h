#ifndef USEFUL_LIB_READ_BINARY_UTIL_H
#define USEFUL_LIB_READ_BINARY_UTIL_H

#include <stdbool.h>
#include <inttypes.h>
#include "endianness.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct BinaryFileResult {
    // Structure status
    bool success;
    int error_code;

    // File properties
    char filepath[2048];
    EndiannessType original_endianness;

    // Parameters
    unsigned int nb_parameters;

    // Elements
    unsigned int elements_type;
    unsigned int elements_size;
    unsigned int nb_elements;

    void * elements;
} BinaryFileResult;

// Endianness tags
#define _BINARY_FILE_ENDIANNESS_LE 0x11
#define _BINARY_FILE_ENDIANNESS_BE 0x22

// Elements type
#define _BINARY_FILE_ELEMENTS_TYPE_DOUBLE 0x11
#define _BINARY_FILE_ELEMENTS_TYPE_INT32 0x22
#define _BINARY_FILE_ELEMENTS_TYPE_INT16 0x23

// Return code
#define _BINARY_ERROR_UNKNOWN_ENDIANNESS 0x847
#define _BINARY_ERROR_NO_MEMORY_AVAILABLE 0x451

BinaryFileResult* binary_file_read_binary(char* filepath);
void binary_file_free_result(BinaryFileResult* result);

void binary_file_print_file_description(BinaryFileResult* result);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif // USEFUL_LIB_ENDIANNESS_UTIL_H
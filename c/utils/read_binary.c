#include "read_binary.h"
#include "endianness.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

BinaryFileResult* binary_file_read_binary(char* filepath) {
    uint32_t nb_parameters;
    uint32_t nb_elements;

    uint8_t file_endianness_int;
    EndiannessType file_endianness;

    uint8_t elements_type;
    uint8_t elements_size;
    uint8_t elements_size_bit;

    double * elements;

    // Allocate result struct
    BinaryFileResult * result = malloc(sizeof(BinaryFileResult));
    if (result == NULL)
        return NULL;

    strcpy(result->filepath, filepath);

    // Open file in READ mode
    FILE* file_ptr = fopen(filepath, "rb");

    // ----------------------------------------------------------------------------------
    // Read file properties
    // ----------------------------------------------------------------------------------
    fread((void *) &file_endianness_int, sizeof(file_endianness_int), 1, file_ptr);

    switch(file_endianness_int) {
        case _BINARY_FILE_ENDIANNESS_LE:
            file_endianness = ENDIANNESS_LITTLE_ENDIAN;
            break;
        case _BINARY_FILE_ENDIANNESS_BE:
            file_endianness = ENDIANNESS_BIG_ENDIAN;
            break;
        default:
            result->success = false;
            result->error_code = _BINARY_ERROR_UNKNOWN_ENDIANNESS;

            return result;
            break;
    }

    result->original_endianness = file_endianness;
    
    // ----------------------------------------------------------------------------------
    // Read parameters
    // ----------------------------------------------------------------------------------
    fread((void *) &nb_parameters, sizeof(uint32_t), 1, file_ptr);
    endianness_fix_endianness((void *) &nb_parameters, 1, sizeof(uint32_t), file_endianness);

    // TODO

    result->nb_parameters = (unsigned int) nb_parameters;

    // ----------------------------------------------------------------------------------
    // Read elements
    // ----------------------------------------------------------------------------------
    fread((void *) &elements_type, 1, 1, file_ptr);

    switch(elements_type) {
        case _BINARY_FILE_ELEMENTS_TYPE_DOUBLE:
            elements_size_bit = 64;
            break;
        case _BINARY_FILE_ELEMENTS_TYPE_INT32:
            elements_size_bit = 32;
            break;
        case _BINARY_FILE_ELEMENTS_TYPE_INT16:
            elements_size_bit = 16;
            break;
        default:
            elements_size_bit = CHAR_BIT;
            break;
    }
    elements_size = (int) (elements_size_bit / CHAR_BIT);

    fread((void *) &nb_elements, 4, 1, file_ptr);
    endianness_fix_endianness((void *) &nb_elements, 1, sizeof(uint32_t), file_endianness);

    elements = (double *) malloc(nb_elements * elements_size);
    if (elements == NULL) {
        result->success = false;
        result->error_code = _BINARY_ERROR_NO_MEMORY_AVAILABLE;

        return result;
    }

    fread((void *) elements, elements_size, nb_elements, file_ptr);

    endianness_fix_endianness((void *) &elements, nb_elements, elements_size, file_endianness);

    // Store elements inside result structure
    result->elements_type = (unsigned int) elements_type;
    result->elements_size = (unsigned int) elements_size;
    result->nb_elements = (unsigned int) nb_elements;
    
    result->elements = (void *) elements;
    result->success = true;
    result->error_code = 0;

    return result;
}

void binary_file_free_result(BinaryFileResult* result) {
    if(result->success)
        free(result->elements);
    
    // TODO: Free parameters

    free(result);
}

void binary_file_print_file_description(BinaryFileResult* result) {
    printf("---------------------------------------------------\n");
    printf("File %s \n", result->filepath);
    printf("---------------------------------------------------\n");
    
    if (result->error_code == 0) {
        printf("Original endianness: %s\n", (result->original_endianness == ENDIANNESS_LITTLE_ENDIAN) ? "LITTLE-ENDIAN" : "BIG-ENDIAN");
        
        printf("Number of parameters: %d\n", result->nb_parameters);
        printf("Number of elements: %d\n", result->nb_elements);
        printf("Elements type: ");

        switch (result->elements_type) {
            case _BINARY_FILE_ELEMENTS_TYPE_DOUBLE:
                printf("DOUBLE (64 bits)");
                break;
            case _BINARY_FILE_ELEMENTS_TYPE_INT32:
                printf("INT32 (32 bits)");
                break;
            case _BINARY_FILE_ELEMENTS_TYPE_INT16:
                printf("INT16 (16 bits)");
                break;
            default:
                printf("UNKNOWN");
                break;
        }

        printf(" [size %d o]\n", result->elements_size);
    }
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#ifndef USEFUL_LIB_ENDIANNESS_UTIL_H
#define USEFUL_LIB_ENDIANNESS_UTIL_H

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef enum EndiannessType {
    ENDIANNESS_LITTLE_ENDIAN,
    ENDIANNESS_BIG_ENDIAN
} EndiannessType;

EndiannessType endianness_get_endianness();
void endianness_change_endianness(char * value, size_t size);
void endianness_fix_endianness(void * values, unsigned int nb_values, size_t values_size, EndiannessType current_endianness);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif // USEFUL_LIB_ENDIANNESS_UTIL_H
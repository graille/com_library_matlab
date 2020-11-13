#include "endianness.h"
#include <limits.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

EndiannessType endianness_get_endianness() {
    int num = 1;

    if (*(char *)&num == 1)
        return ENDIANNESS_LITTLE_ENDIAN;
    else
        return ENDIANNESS_BIG_ENDIAN;
}

void endianness_change_endianness(char * value, size_t size) {
    #if CHAR_BIT == 8
        // Source: https://stackoverflow.com/questions/7279393
        uint16_t v16;
        uint32_t v32;
        uint64_t v64;

        switch(size) {
            case 1:
                return;
            break;
            
            case 2:
                v16 = *((uint16_t *) value);
                *((uint16_t *) value) = (v16 << 8) |
                    ((v16 >> 8) & 0x00ff);
            break;

            case 4:
                v32 = *((uint32_t *) value);
                *((uint32_t *) value) = (v32 << 24) |
                    ((v32 <<  8) & 0x00ff0000) |
                    ((v32 >>  8) & 0x0000ff00) |
                    ((v32 >> 24) & 0x000000ff);
            break;

            case 8:
                // TODO: Check
                v64 = *((uint64_t *) value);
                *((uint64_t *) value) = (v64 << 56) |
                    ((v64 << 32) & 0x00ff000000000000) |
                    ((v64 << 24) & 0x0000ff0000000000) |
                    ((v64 << 8)  & 0x000000ff00000000) |
                    ((v64 >> 8)  & 0x00000000ff000000) |
                    ((v64 >> 24) & 0x0000000000ff0000) |
                    ((v64 >> 32) & 0x000000000000ff00) |
                    ((v64 >> 56) & 0x00000000000000ff);
            break;
        }
    #else
        #error "Unsupported CHAR_BIT value"
    #endif
}

void endianness_fix_endianness(void * values, unsigned int nb_values, size_t values_size, EndiannessType current_endianness) {
    /**
        Convert a value described by "values" from "current_endianness" to system endianness
    **/

    unsigned int i;
    if (current_endianness != endianness_get_endianness()) {
        for (i = 0; i < nb_values; i++) {
            endianness_change_endianness((char *) (values + i*values_size), values_size);
        }
    }
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
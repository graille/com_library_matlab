#ifndef USEFUL_LIB_CONCATENATE_UTIL_H
#define USEFUL_LIB_CONCATENATE_UTIL_H
 
#include <stdlib.h>
#include <math.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef enum OperationMode {
    SINGLE,
    ARRAY
} OperationMode;

size_t concatenate_tool_get_output_size(const unsigned char * Pi, int nb_elements, OperationMode mode);

unsigned int concatenate_tool_create_mask(int start, int size);
void concatenate_tool_concatenate(const unsigned int * elements, const unsigned char * Pi, int input_size, unsigned char * output_elements, int output_size, OperationMode mode);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif // USEFUL_LIB_CONCATENATE_UTIL_H